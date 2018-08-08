// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopDefinitions.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 bDebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.Debug.Weapons"),
	bDebugWeaponDrawing,
	TEXT("Draw debug lines for weapons"),
	ECVF_Cheat
);

// Sets default values
ACoopWeapon::ACoopWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";		// Name of the parameter for the ending location of an effect that works like a trace bettwen two points

	BaseDamage = 20.0f;
	HeadshotDamageMultiplier = 4.0f;

	FireRate = 600.0f;
	
	// As weird as it is, we need to increase the frequency this updates in network, otherwise the shots are too far betweens
	// This values are similar to Counter Strike. We want our game to be responsive in network.
	NetUpdateFrequency = 70.0f;
	MinNetUpdateFrequency = 33.0f;

	SetReplicates(true);
}

void ACoopWeapon::PlayFireEffect(FVector TraceHit)
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);

	FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

	if (TraceEffect)
	{
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
		TracerComponent->SetVectorParameter(TracerTargetName, TraceHit);
	}

	if (FireCameraShakeClass)
	{
		APawn* MyOwner = Cast<APawn>(GetOwner());
		if (MyOwner)
		{
			APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
			if (PC)
			{
				PC->ClientPlayCameraShake(FireCameraShakeClass);
			}
		}
	}
}

void ACoopWeapon::Fire()
{
	// If a client machine, lets ask the server to fire from there.
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}

	// Trace from the pawn eyes to the crosshair
	AActor* PawnOwner = GetOwner();
	if (PawnOwner)
	{
		FVector		EyeLocation;
		FRotator	EyeRotation;
		PawnOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		
		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 1000.0f);
		FHitResult Hit;
		
		// Particle trace "Target" parameter. By default the trace just goes to its extreme without hitting anything
		FVector TraceHit = TraceEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PawnOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		
		EPhysicalSurface SurfaceType= SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit, process damage

			AActor* HitActor = Hit.GetActor();

			TraceHit = Hit.ImpactPoint;

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == ECC_COOP_PHYSUR_FLESHSENSITIVE)
			{
				ActualDamage *= HeadshotDamageMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, PawnOwner->GetInstigatorController(), this, CoopWeaponDamage);
			
			if (ImpactEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint);
		}

		if (bDebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffect(TraceHit);

		if (Role == ROLE_Authority)
		{
			HitScanData.TraceTo= TraceHit;
			HitScanData.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ACoopWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic effect
	PlayFireEffect(HitScanData.TraceTo);
	PlayImpactEffect(HitScanData.SurfaceType, HitScanData.TraceTo);
}

void ACoopWeapon::PlayImpactEffect(const EPhysicalSurface& SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SurfaceType_Default:
		SelectedEffect = ImpactEffect;
		break;
	case ECC_COOP_PHYSUR_FLESHREGULAR:
		SelectedEffect = FleshEffect;
		break;
	case ECC_COOP_PHYSUR_FLESHSENSITIVE:
		SelectedEffect = FleshSensitiveEffect;
		break;
	default:
		SelectedEffect = ImpactEffect;
		break;
	}

	const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

	FVector ShotDirection = ImpactPoint - MuzzleLocation;
	ShotDirection.Normalize();

	if (SelectedEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
}

void ACoopWeapon::ServerFire_Implementation()
{
	Fire();
	OnRep_HitScanTrace(); // To make sure the effects of the hitscan are called in server too.
}

bool ACoopWeapon::ServerFire_Validate()
{
	return true;
}

void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBettwenShots = 60.0 / FireRate;
}

void ACoopWeapon::StartFire()
{
	// To avoid spaming for ignoring the fire rate, we calculate a delay, if enough time has passed since the last shot it will be possible to make the shot.
	const float FirstDelay = FMath::Max(LastFireTime + TimeBettwenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(FireTimer, this, &ACoopWeapon::Fire, TimeBettwenShots, true, FirstDelay);
}

void ACoopWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void ACoopWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACoopWeapon, HitScanData, COND_SkipOwner); // The effect will play in the owner, we only want to replicate it in all other machines
}
