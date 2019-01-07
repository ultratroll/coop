// Fill out your copyright notice in the Description page of Project Settings.

#include "ASAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"

UASAttributeSetBase::UASAttributeSetBase()
	:Health(200.0f)
{
}

void UASAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UASAttributeSetBase::StaticClass(), GET_MEMBER_NAME_CHECKED(UASAttributeSetBase, Health)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage took: %f"), Health.GetCurrentValue());
	}
}
