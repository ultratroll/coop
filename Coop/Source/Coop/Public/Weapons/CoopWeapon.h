// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TraceEffect;

	/** Name for the socket of the weapon. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	/** Name of the parametter in the trail effect to design the destination location of the trail effect. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName TracerTargetName;

	UFUNCTION()
	void PlayFireEffect(FVector TraceHit);

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> CoopWeaponDamage;

	/** Fire the weapon. */
	UFUNCTION(BlueprintCallable, Category= "Weapon")
	virtual void Fire();
};
