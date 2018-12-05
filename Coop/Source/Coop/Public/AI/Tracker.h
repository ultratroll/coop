// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tracker.generated.h"

class UStaticMeshComponent;
class UCoopHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS( ClassGroup=(Coop), Blueprintable )
class COOP_API ATracker : public APawn
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category= "Debug")
	uint8 bDebugTracker : 1;

	// Sets default values for this pawn's properties
	ATracker();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Audio")
	USoundCue * SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	USoundCue * CharacterSpottedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	USoundCue * ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic * MaterialInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	UCoopHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	USphereComponent* DamageSphere;

	/** Next point to move to. */
	FVector NextPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker|Swarm boost")
	float FrequencyCheckNearTrackers;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker|Swarm boost")
	float DistanceCheckNearTrackers;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker|Explosion")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker|Explosion")
	float ExplosionRadius;

	/** Once character is detected, how frequently the tracker will self harm until exploding. */
	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float SelfDamageFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float RequiredDistanceToTarget;

	/** How many trackers are nearby, this will set the maximum power level.*/
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Power")
	int PowerLevel;

	UPROPERTY(EditDefaultsOnly, Category= "Tracker")
	uint8 bUseVelocityChange : 1;

	UPROPERTY(Transient)
	uint8 bExploded : 1;

	UPROPERTY(Transient)
	uint8 bStartedSelfDestruct : 1;

	/** */
	UPROPERTY()
	FTimerHandle TimerRefreshPath;

	/** If the tracker touches a coopcharacter, we want it to self damage until exploding, instead of exploding instantly. */
	UPROPERTY()
	FTimerHandle TimerSelfDamage;

	/** Timer to check for nearby trackers. */
	UPROPERTY()
	FTimerHandle TimerNearbyTrackers;

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

	UFUNCTION()
	void DamageSelf();

	UFUNCTION()
	void CheckNearTrackers();

	UFUNCTION()
	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
