// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";		// Name of the parameter for the ending location of an effect that works like a trace bettwen two points
}

// Called when the game starts or when spawned
void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			// Blocking hit, process damage

			AActor* HitActor = Hit.GetActor();

			TraceHit = Hit.ImpactPoint;

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, PawnOwner->GetInstigatorController(), this, CoopWeaponDamage);
			
			if (ImpactEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		if (MuzzleEffect)
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);

		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		if (TraceEffect)
		{
			UParticleSystemComponent* TracerComponent= UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
			TracerComponent->SetVectorParameter(TracerTargetName, TraceHit);
		}
	}
}

