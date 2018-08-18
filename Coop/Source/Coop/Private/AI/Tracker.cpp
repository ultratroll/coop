// Fill out your copyright notice in the Description page of Project Settings.

#include "Tracker.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "CoopHealthComponent.h"

/**
 *	Simple Tracker.
 */

// Sets default values
ATracker::ATracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetSimulatePhysics(true);

	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("Health Component"));
	HealthComponent->GetOnHealthChanged().AddDynamic(this, &ATracker::OnHealthChanged);

	MovementForce = 1000.0f;
	RequiredDistanceToTarget = 100.0f;
	bUseVelocityChange = false;
}

// Called when the game starts or when spawned
void ATracker::BeginPlay()
{
	Super::BeginPlay();

	SetCanAffectNavigationGeneration(false);

	// Find initial point to move to.
	NextPoint = GetNextPathPoint();
}

const FVector ATracker::GetNextPathPoint()
{
	// Hack to get player location
	ACharacter* Character= UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), Character);

	// The first point in the path is always the current location, we want the second one to know where to push
	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ATracker::OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// TODO:: When health reaches 0, lets explode

	// TODO:: lets pulsate the material

	UE_LOG(LogTemp, Warning, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *(DamageCauser->GetName()));
}

void ATracker::Explode()
{

}

// Called every frame
void ATracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float DistanceToTarget = (GetActorLocation() - NextPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target reached",(AActor*)0, FColor::Green, 0, false);
	}
	else
	{
		// Keep moving.
		FVector ForceDirection = NextPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComponent->AddImpulse(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation()+ForceDirection, 32.0f, FColor::Blue, false, 0, 0, 1);
	}

	DrawDebugSphere(GetWorld(), NextPoint, 20.0f, 12, FColor::Yellow, false,0, 0, 1);
}
