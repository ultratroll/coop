// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopCharacter.h"
#include "CoopWeapon.h"
#include "Camera/CameraComponent.h"
#include "CoopDefinitions.h"
#include "CoopHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

	// We want weapons shots to hit our character mesh, not the capsule
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_COLLISION_WEAPON, ECR_Ignore);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	WeaponAttachSocketName = "WeaponSocketR";

	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComponent"));

	TargetZoom = 65.0f;
	ZoomInterpolationSpeed = 20.0f;
}

// Called when the game starts or when spawned
void ACoopCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultZoom = CameraComponent->FieldOfView;
	HealthComponent->GetOnHealthChanged().AddDynamic(this, &ACoopCharacter::OnHealthChanged);

	// Only spawn in the server machine
	if (Role= ROLE_Authority)
	{
		// Spawn a default weapon
		if (DefaultWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			EquipedWeapon = GetWorld()->SpawnActor<ACoopWeapon>(DefaultWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

			if (EquipedWeapon)
			{
				EquipedWeapon->SetOwner(this);
				EquipedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			}
		}
	}
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

void ACoopCharacter::BeginZoom()
{
	bWantsToZoom= true;
}

void ACoopCharacter::EndZoom()
{
	bWantsToZoom= false;
}

void ACoopCharacter::OnFireBegin()
{
	if (IsValid(EquipedWeapon))
	{
		EquipedWeapon->StartFire();
	}
}

void ACoopCharacter::OnFireEnd()
{
	if (IsValid(EquipedWeapon))
	{
		EquipedWeapon->StopFire();
	}
}

void ACoopCharacter::OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bDied)
	{
		// Die!
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);

		// Ragdoll later
	}
}

// Called every frame
void ACoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float const DessiredZoom = bWantsToZoom ? TargetZoom : DefaultZoom;
	float const InterpolatedZoom = FMath::FInterpTo(CameraComponent->FieldOfView, DessiredZoom, DeltaTime, ZoomInterpolationSpeed);
	CameraComponent->SetFieldOfView(InterpolatedZoom);
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

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ACoopCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ACoopCharacter::EndZoom);

	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACoopCharacter::Jump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACoopCharacter::OnFireBegin);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACoopCharacter::OnFireEnd);
}

FVector ACoopCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
		return CameraComponent->GetComponentLocation();

	return Super::GetPawnViewLocation();
}

void ACoopCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopCharacter, EquipedWeapon);
}
