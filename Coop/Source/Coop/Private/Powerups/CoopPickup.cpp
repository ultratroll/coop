// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPickup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "CoopCharacter.h"
#include "CoopPowerup.h"

// Sets default values
ACoopPickup::ACoopPickup()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereComponent->SetSphereRadius(76.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	DecalComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComponent->SetupAttachment(RootComponent);

	RespawnPowerupTime = 5.0f;
}

// Called when the game starts or when spawned
void ACoopPickup::BeginPlay()
{
	Super::BeginPlay();

	SpawnPowerup();
}

void ACoopPickup::SpawnPowerup()
{
	FActorSpawnParameters SpawnData;
	SpawnData.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = (IsValid(PowerupClass))? GetWorld()->SpawnActor<ACoopPowerup>(PowerupClass, GetTransform(), SpawnData) : nullptr;
}

void ACoopPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (PowerupInstance)
	{
		ACoopCharacter* const Character = Cast<ACoopCharacter>(OtherActor);
		PowerupInstance->Activate(Character);
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerPickupTick, this, &ACoopPickup::SpawnPowerup, RespawnPowerupTime);
	}
}
