// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

/**
 * Contains information on a single hitscan weapon line trace
 */
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FVector_NetQuantize TraceFrom;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOP_API ACoopWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopWeapon();

protected:

	UPROPERTY(EditAnywhere, Category= "Components")
	USkeletalMeshComponent * MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Impact Effect")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Impact Effect")
	UParticleSystem* FleshEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Impact Effect")
	UParticleSystem* FleshSensitiveEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TraceEffect;

	/** Name for the socket of the weapon. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	/** Name of the parameter in the trail effect to design the destination location of the trail effect. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadshotDamageMultiplier;

	FTimerHandle FireTimer;

	/** Last time weapon was fired. */
	float LastFireTime;

	/** RPM, Bullets per minute. */
	UPROPERTY(EditDefaultsOnly)
	float FireRate;

	/** Based on firerate. */
	UPROPERTY()
	float TimeBettwenShots;

	UFUNCTION()
	void PlayFireEffect(FVector TraceHit);

	/** Fire the weapon. */
	virtual void Fire();

	/** Fire the weapon from server. */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UPROPERTY(ReplicatedUsing= OnRep_HitScanTrace)
	FHitScanTrace HitScanData;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:	

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> CoopWeaponDamage;

	void StartFire();

	void StopFire();
};
