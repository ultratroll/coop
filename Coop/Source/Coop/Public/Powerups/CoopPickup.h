// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPickup.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS()
class COOP_API ACoopPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopPickup();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent * SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UDecalComponent* DecalComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;	
};
