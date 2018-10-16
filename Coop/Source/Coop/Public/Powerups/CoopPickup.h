// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPickup.generated.h"

class USphereComponent;
class UDecalComponent;
class ACoopPowerup;

UCLASS()
class COOP_API ACoopPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopPickup();

protected:

	UPROPERTY(Transient)
	FTimerHandle TimerPickupTick;

	UPROPERTY(EditDefaultsOnly, Category= "Settings")
	float RespawnPowerupTime;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent * SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Powerup")
	TSubclassOf<ACoopPowerup> PowerupClass;

	UPROPERTY(Transient, BlueprintReadOnly)
	ACoopPowerup* PowerupInstance;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnPowerup();

public:	
	
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;	
};
