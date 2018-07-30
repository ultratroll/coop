// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopExplosiveBarrel.h"
#include "CoopHealthComponent.h"
#include "Components/MeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACoopExplosiveBarrel::ACoopExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComponent"));

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false; // We want it to activate only when its supossed to
	RadialForceComponent->ForceStrength = ForceExplosion;
	RadialForceComponent->Radius = RadiusExplosion;
	RadialForceComponent->SetupAttachment(MeshComponent);
	RadialForceComponent->SetRelativeLocation(FVector::ZeroVector);
	RadialForceComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

// Called when the game starts or when spawned
void ACoopExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->GetOnHealthChanged().AddDynamic(this, &ACoopExplosiveBarrel::OnHealthChanged);
}

void ACoopExplosiveBarrel::Explode()
{
 	RadialForceComponent->FireImpulse();

	MeshComponent->AddImpulse(GetActorUpVector() * ForceExplosion, NAME_None, true);

	if (ExplosionEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());

	if (MaterialBarrelExploded)
		MeshComponent->SetMaterial(0, MaterialBarrelExploded);
}

void ACoopExplosiveBarrel::OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bIsExploding)
	{
		// Die!
		bIsExploding = true;
		
		Explode();

		SetLifeSpan(10.0f);

		// Ragdoll later
	}
}