// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoopHealthComponent.generated.h"

/**
 *	Component to manage health.
 */

UCLASS( ClassGroup=(Coop), meta=(BlueprintSpawnableComponent) )
class COOP_API UCoopHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoopHealthComponent();

protected:

	UPROPERTY(Transient, BlueprintReadOnly, Category= "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth= 100.0f;

	// Called when the game starts
	virtual void BeginPlay() override;

	/** When health changes. */
	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	

};
