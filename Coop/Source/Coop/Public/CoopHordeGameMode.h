// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoopHordeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOP_API ACoopHordeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	
protected:

	int32 WaveCount;

	int32 BotsToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Horde")
	float TimeBettwenWaves;

	FTimerHandle TimerHandleSpawnBots;

	FTimerHandle TimerHandleStartWave;

protected:

	/** Spawn new bot in blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horde")
	void SpawnBot();

	UFUNCTION()
	void TrySpawnBot();
	
	/** Begin spawning bots. */
	void StartWave();

	/** Stop spawning bots. */
	void EndWave();

	/** Set timer for next start wave. */
	void PrepareForNextWave();

public:

	CoopHordeGameMode();

	virtual void StartPlay() override;
};
