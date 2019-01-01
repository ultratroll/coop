// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ASCharacter.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
class UASAttributeSetBase;

UCLASS()
class ABILITY_API AASCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	// Sets default values for this character's properties
	AASCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilityCharacter", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilityCharacter", meta = (AllowPrivateAccess = "true"))
	UASAttributeSetBase* AtributeSet;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AbilityCharacter")
	void AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire);
};
