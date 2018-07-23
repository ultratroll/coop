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
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit, process damage

			AActor* HitActor = Hit.GetActor();

			TraceHit = Hit.ImpactPoint;

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == ECC_COOP_PHYSUR_FLESHSENSITIVE)
			{
				ActualDamage *= HeadshotDamageMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, PawnOwner->GetInstigatorController(), this, CoopWeaponDamage);
			
			if (ImpactEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			

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

			if (SelectedEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		if (bDebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffect(TraceHit);
	}
}

