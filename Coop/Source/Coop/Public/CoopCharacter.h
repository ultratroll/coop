// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoopCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ACoopWeapon;
class UCoopHealthComponent;

UCLASS()
class COOP_API ACoopCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoopCharacter();

protected:

	uint8 bWantsToZoom : 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category= "Player")
	uint8 bDied : 1;

	float DefaultZoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", meta= (ClampMin= 0.1f, ClampMax= 100.0f))
	float ZoomInterpolationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom")
	float TargetZoom;

	/** Component to manage health. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCoopHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ACoopWeapon> DefaultWeaponClass;

	UPROPERTY(Replicated)
	ACoopWeapon* EquipedWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category= "Weapon")
	FName WeaponAttachSocketName;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	void OnFireBegin();

	void OnFireEnd();

	UFUNCTION()
	void OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const override;
	
};
