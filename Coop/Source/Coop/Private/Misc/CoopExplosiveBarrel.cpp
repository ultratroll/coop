// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopExplosiveBarrel.h"
#include "CoopHealthComponent.h"
#include "Components/MeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ACoopExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->GetOnHealthChanged().AddDynamic(this, &ACoopExplosiveBarrel::OnHealthChanged);
}

void ACoopExplosiveBarrel::ExplodeVissualEffect()
{
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

		// We want to have this impulse happening on the server (server, remember that the health component is only used if there is authority)
		RadialForceComponent->FireImpulse();
		MeshComponent->AddImpulse(GetActorUpVector() * ForceExplosion, NAME_None, true);

		// Since its server, lets make sure to call this here
		OnRep_Explosion();

		SetLifeSpan(10.0f);

		// Ragdoll later
	}
}

void ACoopExplosiveBarrel::OnRep_Explosion()
{
	ExplodeVissualEffect();
}

void ACoopExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopExplosiveBarrel, bIsExploding);
}