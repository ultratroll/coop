// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

// Sets default values
ACoopWeapon::ACoopWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopWeapon::Fire()
{
	// Trace from the pawn eyes to the crosshair
	AActor* PawnOwner = GetOwner();
	if (PawnOwner)
	{
		FVector		EyeLocation;
		FRotator	EyeRotation;
		PawnOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		
		FVector ShotDirection = EyeRotation.Vector();
		FVector EndLocation = EyeLocation + ShotDirection * 1000.0f;
		FHitResult Hit;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PawnOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			// Blocking hit, process damage

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, PawnOwner->GetInstigatorController(), this, CoopWeaponDamage);
		}

		DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::White, false, 1);
	}
}

