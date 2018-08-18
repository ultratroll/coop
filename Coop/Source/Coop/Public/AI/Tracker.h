// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tracker.generated.h"

class UStaticMeshComponent;
class UCoopHealthComponent;

UCLASS(Blueprintable)
class COOP_API ATracker : public APawn
{
	GENERATED_BODY()

public:
	
	// Sets default values for this pawn's properties
	ATracker();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	UCoopHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** Next point to move to. */
	FVector NextPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category= "Tracker")
	uint8 bUseVelocityChange : 1;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	*	We need the path points to know where to move, since the tracker will move using forces.
	*/
	const FVector GetNextPathPoint();

	UFUNCTION()
	void OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
};
