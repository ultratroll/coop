// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/CoopWeapon.h"
#include "CoopWeaponLauncher.generated.h"

class ACoopProjectile;

/**
 * 
 */
UCLASS()
class COOP_API ACoopWeaponLauncher : public ACoopWeapon
{
	GENERATED_BODY()
	
public:
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ACoopProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	USoundBase* FireSound;

	/** Fire the weapon. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() override;
	
};
