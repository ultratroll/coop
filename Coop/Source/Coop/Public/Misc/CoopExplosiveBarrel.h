// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopExplosiveBarrel.generated.h"

class UCoopHealthComponent;
class URadialForceComponent;

UCLASS(Blueprintable)
class COOP_API ACoopExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopExplosiveBarrel();

protected:

	/** Is explosing ?. */
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsExploding : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	/** Component to manage health. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCoopHealthComponent * HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	UMaterialInterface* MaterialBarrelExploded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float ForceExplosion= 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	float RadiusExplosion = 250.0f;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Explode();

	UFUNCTION()
	void OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:		
	
};
