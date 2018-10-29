// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPowerup.generated.h"

class ACoopCharacter;
class USoundCue;
class UParticleSystem;

UCLASS()
class COOP_API ACoopPowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopPowerup();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActivated)
	uint8 bIsActivated;

	/** The powerup effect has a single use?, otherwise it will be applied each PowerupInterval for TotalNumberTicks times. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	uint8 bIsSingleUse : 1;

	/** Total time we apply the powerup effect. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (EditCondition = "!bIsSingleUse"))
	int32 TotalNumberTicks;

	UPROPERTY(Transient)
	int32 TicksProcessed;

	/** Time bettwen applications of the powerup effect. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (EditCondition = "!bIsSingleUse"))
	float PowerupInterval;

	/** Single use duration. */
	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (EditCondition = "bIsSingleUse"))
	float SingleUseDuration;

	UPROPERTY(Transient)
	FTimerHandle TimerPowerupTick;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, Transient)
	ACoopCharacter* UserInstigator;

	/** Particle effect for whenever powerup is picked. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	UParticleSystem* PickedParticleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	USoundCue* PickedSound;

	/** If the powerup is tick based, called on tick. */
	void OnTickPowerup();

	/** Play sound and particle effects when picked. */
	UFUNCTION()
	void PlayPickedEffect();

	UFUNCTION()
	void OnRep_PowerUpActivated();

	/** Only used when finishing a powerup (tick ended or single use ended). Does the cleaning up. */
	UFUNCTION()
	void OnFinishUse();

public:

	UFUNCTION(BlueprintCallable)
	void SetInstigator(ACoopCharacter* NewInstigator)  { UserInstigator = NewInstigator; }

	/** Activate the powerup. */
	UFUNCTION(BlueprintCallable, Category= "Powerup")
	void Activate(ACoopCharacter* NewInstigator);

	/** Called only when the powerup starts to be applied, useful to catch for example starting values. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated();

	/** Called only when the powerup repeats an effect by tick, and those end. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

	/** Here the powerup is actualy applied, its effect. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();
};
