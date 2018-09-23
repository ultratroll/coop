// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCoopHealthComponent::UCoopHealthComponent()
{
	DefaultHealth= 100.0f;

	SetIsReplicated(true);
}

void UCoopHealthComponent::OnRep_HealthChanged(float OldHealth)
{
	float Damage = OldHealth - Health;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, GetOwner());
}

// Called when the game starts
void UCoopHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole()== ROLE_Authority) // Only in server
	{
		AActor* MyOwner = GetOwner();
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UCoopHealthComponent::OnDamage);
	}
	
	Health = DefaultHealth;
}

void UCoopHealthComponent::OnDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0)
		return;

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UCoopHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCoopHealthComponent, Health);
}
