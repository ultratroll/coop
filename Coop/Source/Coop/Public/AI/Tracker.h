// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tracker.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class COOP_API ATracker : public APawn
{
	GENERATED_BODY()

public:
	
	// Sets default values for this pawn's properties
	ATracker();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	*	We need the path points to know where to move, since the tracker will move using forces.
	*/
	FVector GetNextPathPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
