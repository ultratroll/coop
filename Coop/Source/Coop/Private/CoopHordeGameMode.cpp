// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHordeGameMode.h"
#include "TimerManager.h"


ACoopHordeGameMode::ACoopHordeGameMode()
{
	TimeBettwenWaves = 10.0f;
}

void ACoopHordeGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ACoopHordeGameMode::TrySpawnBot()
{
	SpawnBot();

	BotsToSpawn--;

	if (BotsToSpawn <= 0)
		EndWave();
}

void ACoopHordeGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("STARTING WAVE"));

	WaveCount++;

	BotsToSpawn = 2 *WaveCount;

	GetWorldTimerManager().ClearTimer(TimerHandleStartWave);

	GetWorldTimerManager().SetTimer(TimerHandleSpawnBots, this, &ACoopHordeGameMode::TrySpawnBot, 1.0f, true, 0.0f);
}

void ACoopHordeGameMode::EndWave()
{
	UE_LOG(LogTemp, Warning, TEXT("ENDING WAVE"));

	GetWorldTimerManager().ClearTimer(TimerHandleSpawnBots);

	PrepareForNextWave();
}

void ACoopHordeGameMode::PrepareForNextWave()
{
	UE_LOG(LogTemp, Warning, TEXT("NEXT WAVE IN %f"), TimeBettwenWaves);

	GetWorldTimerManager().SetTimer(TimerHandleStartWave, this, &ACoopHordeGameMode::StartWave, TimeBettwenWaves, false);
}
