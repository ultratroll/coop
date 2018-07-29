// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHealthComponent.h"

// Sets default values for this component's properties
UCoopHealthComponent::UCoopHealthComponent()
{
	DefaultHealth= 100.0f;
}


// Called when the game starts
void UCoopHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;

	AActor* MyOwner = GetOwner();
	MyOwner->OnTakeAnyDamage.AddDynamic(this, &UCoopHealthComponent::OnDamage);
	
}

void UCoopHealthComponent::OnDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
		return;

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}
