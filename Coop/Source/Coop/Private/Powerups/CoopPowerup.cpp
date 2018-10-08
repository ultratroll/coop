// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopPowerup.h"


// Sets default values
ACoopPowerup::ACoopPowerup()
{
	PowerupInterval = 0;
	TotalNumberTicks = 0;
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
		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerPowerupTick);
	}
}

void ACoopPowerup::ActivatePowerup()
{
	if (PowerupInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimerPowerupTick, this, &ACoopPowerup::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		ApplyPowerup();
	}
}

