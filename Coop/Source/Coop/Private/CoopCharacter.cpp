// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ACoopCharacter::ACoopCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraArm);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called when the game starts or when spawned
void ACoopCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoopCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector()*Value);
}

void ACoopCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector()*Value);
}

void ACoopCharacter::BeginCrouch()
{
	Crouch();
}

void ACoopCharacter::EndCrouch()
{
	UnCrouch();
}

// Called every frame
void ACoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACoopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACoopCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACoopCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACoopCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACoopCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACoopCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACoopCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACoopCharacter::Jump);
}

FVector ACoopCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
		return CameraComponent->GetComponentLocation();

	return Super::GetPawnViewLocation();
}

