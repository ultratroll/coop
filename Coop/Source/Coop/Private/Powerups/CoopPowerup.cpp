// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPowerup.h"
#include "CoopCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ACoopPowerup::ACoopPowerup()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetupAttachment(RootComponent);

	bIsSingleUse = true;
	SingleUseDuration = 5.0f;
	TotalNumberTicks = 5;
	PowerupInterval = 1.0f;
}

// Called when the game starts or when spawned
void ACoopPowerup::BeginPlay()
{
	Super::BeginPlay();
}

void ACoopPowerup::OnTickPowerup()
{
	TicksProcessed++;

	ApplyPowerup();

	if (TicksProcessed >= TotalNumberTicks)
	{
		GetWorldTimerManager().ClearTimer(TimerPowerupTick);
		OnExpired();
	}
}


void ACoopPowerup::PlayPickedEffect()
{
	if (PickedParticleEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickedParticleEffect, GetActorLocation(), GetActorRotation());

	if (PickedSound)
		UGameplayStatics::SpawnSoundAttached(PickedSound, RootComponent);
}

void ACoopPowerup::Activate(ACoopCharacter* NewInstigator)
{
	SetInstigator(NewInstigator);

	GetWorldTimerManager().ClearTimer(TimerPowerupTick);

	PlayPickedEffect();

	OnActivated();

	MeshComponent->SetVisibility(false);

	if (bIsSingleUse == 0)
	{
		TicksProcessed = 0;
		GetWorldTimerManager().SetTimer(TimerPowerupTick, this, &ACoopPowerup::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		ApplyPowerup();

		GetWorldTimerManager().SetTimer(TimerPowerupTick, this, &ACoopPowerup::OnExpired, SingleUseDuration);
	}
}

