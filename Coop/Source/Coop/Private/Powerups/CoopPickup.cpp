// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPickup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

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
}

// Called when the game starts or when spawned
void ACoopPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoopPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// TODO::
}
