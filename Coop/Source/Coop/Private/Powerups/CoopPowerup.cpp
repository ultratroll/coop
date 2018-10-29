// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPowerup.h"
#include "CoopCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
}

void ACoopPowerup::OnTickPowerup()
{
	if (HasAuthority())
	{
		TicksProcessed++;

		OnPowerupTicked();

		if (TicksProcessed >= TotalNumberTicks)
		{
			OnFinishUse();
		}
	}
}


void ACoopPowerup::PlayPickedEffect()
{
	if (PickedParticleEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickedParticleEffect, GetActorLocation(), GetActorRotation());

	if (PickedSound)
		UGameplayStatics::SpawnSoundAttached(PickedSound, RootComponent);
}

void ACoopPowerup::OnRep_PowerUpActivated()
{
	if (bIsActivated)
	{
		MeshComponent->SetVisibility(false);
		PlayPickedEffect();
	}
	else
	{
		;
	}
}

void ACoopPowerup::OnFinishUse()
{
	OnExpired();

	GetWorldTimerManager().ClearTimer(TimerPowerupTick);
	OnExpired();
	bIsActivated = false;
	OnRep_PowerUpActivated();
	Destroy();
}

void ACoopPowerup::Activate(ACoopCharacter* NewInstigator)
{
	if (HasAuthority())
	{
		SetInstigator(NewInstigator);

		bIsActivated = true;
		OnRep_PowerUpActivated();

		GetWorldTimerManager().ClearTimer(TimerPowerupTick);

		OnActivated();

		if (bIsSingleUse == 0)
		{
			TicksProcessed = 0;
			GetWorldTimerManager().SetTimer(TimerPowerupTick, this, &ACoopPowerup::OnTickPowerup, PowerupInterval, true, 0.0f);
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerPowerupTick, this, &ACoopPowerup::OnFinishUse, SingleUseDuration);
		}
	}
}

void ACoopPowerup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopPowerup, bIsActivated);
	DOREPLIFETIME(ACoopPowerup, Instigator);
}

