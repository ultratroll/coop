// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CoopHordeGameState.generated.h"

UENUM(BlueprintType)
enum class EHordeState : uint8
{
	WaitingToStart,

	WaveInProgress,

	/** No longer spawning new bots, waiting for players to kill remaining bots. */
	WaitingToComplete,

	WaveComplete,

	GameOver
};

/**
 *	Horde state for our horde game.
 */
UCLASS()
class COOP_API ACoopHordeGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnRep_HordeState(EHordeState OldHordeState);

	UFUNCTION(BlueprintImplementableEvent, Category= "Horde")
	void OnWaveStateChanged(EHordeState OldHordeState, EHordeState NewHordeState);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HordeState, Category = "Horde")
	EHordeState HordeState;
	
public:

	UFUNCTION()
	void SetHordeState(EHordeState NewHordeState);

};
