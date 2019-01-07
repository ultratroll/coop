// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class ABILITY_API UASAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:

	UASAttributeSetBase();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeBase")
	FGameplayAttributeData  Health;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
};
