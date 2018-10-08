// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPowerup.generated.h"

UCLASS()
class COOP_API ACoopPowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopPowerup();

protected:

	UPROPERTY(EditDefaultsOnly, Category= "Settings")
	float PowerupInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 TotalNumberTicks;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category= "Powerup")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();
};
