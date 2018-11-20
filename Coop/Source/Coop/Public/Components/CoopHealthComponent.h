// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoopHealthComponent.generated.h"

/**
 *	Component to manage health.
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChanged, UCoopHealthComponent*, HealthComponent, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Coop), meta=(BlueprintSpawnableComponent) )
class COOP_API UCoopHealthComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:	
	
	// Sets default values for this component's properties
	UCoopHealthComponent();

	FORCEINLINE FOnHealthChanged& GetOnHealthChanged() { return OnHealthChanged; }

protected:

	UPROPERTY()
	uint8 bIsDead : 1;

	UPROPERTY(ReplicatedUsing= OnRep_HealthChanged, Transient, BlueprintReadOnly, Category= "HealthComponent")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth= 100.0f;

	UPROPERTY(BlueprintAssignable, Category= "Events")
	FOnHealthChanged OnHealthChanged;

protected:

	UFUNCTION()
	void OnRep_HealthChanged(float OldHealth);

	// Called when the game starts
	virtual void BeginPlay() override;

	/** When health changes. */
	UFUNCTION()
	void OnDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	

	UFUNCTION(BlueprintCallable, Category= "HealthComponent")
	void Heal(float HealAmmount);

	UFUNCTION()
	FORCEINLINE bool IsDead() const { return (Health <= 0); }

	UFUNCTION()
	FORCEINLINE float GetHealth() const { return Health; }

};
