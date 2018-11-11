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

	/** Class of the bots. Notice that currently its centered here, but in the future we will want to have wave structures with multiple types of enemies. */
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Horde")
 	TSubclassOf<APawn> BotClass;
	
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

	/** Used to check how the wave goes, if all enemies have died, then its time to end the wave */
	void CheckForWaveState();

public:

	ACoopHordeGameMode();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartPlay() override;
};
