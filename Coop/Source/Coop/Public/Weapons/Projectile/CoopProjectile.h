// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopProjectile.generated.h"


class UProjectileMovementComponent;
class USphereComponent;


UCLASS()
class COOP_API ACoopProjectile : public AActor
{
	GENERATED_BODY()

	/** initial setup */
	virtual void PostInitializeComponents() override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	uint8 bExplodeOnHit : 1;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float TimeForExplosion= 1.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ExplosionDamage = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	USkeletalMeshComponent * MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UParticleSystem* ImpactEffect;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<UDamageType> CoopWeaponDamage;

	/** Controller that fired this projectile (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

public:

	UPROPERTY()
	FTimerHandle ExplosionTimer;

	ACoopProjectile();

	/** called when projectile hits something */
	UFUNCTION(Category = "Projectile")
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE void SetDamageType(TSubclassOf<UDamageType> DamageType) { CoopWeaponDamage = DamageType; }

	UFUNCTION(Category = "Projectile")
	void Explode();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

