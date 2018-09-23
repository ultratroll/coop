// Fill out your copyright notice in the Description page of Project Settings.

#include "Tracker.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "CoopHealthComponent.h"
#include "CoopCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "CoopDefinitions.h"
#include "Sound/SoundCue.h"

/**
 *	Simple Tracker.
 */

// Sets default values
ATracker::ATracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionObjectType(ECC_ENEMY);
	//MeshComponent->SetCollisionResponseToChannel(ECC_ENEMY_TRACE, ECR_Block); // NOT SURE IF WE ARE GOING TO NEED AN ENEMY TRACE
	RootComponent = MeshComponent;

	MeshComponent->SetSimulatePhysics(true);

	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("Health Component"));
	HealthComponent->GetOnHealthChanged().AddDynamic(this, &ATracker::OnHealthChanged);

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageTrigger"));
	DamageSphere->SetSphereRadius(200.0f);
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DamageSphere->SetupAttachment(RootComponent);

	MovementForce = 1000.0f;
	RequiredDistanceToTarget = 100.0f;
	bUseVelocityChange = false;

	ExplosionDamage = 40.0f;
	ExplosionRadius= 200.0f;
	SelfDamageFrequency = 0.5f;

	FrequencyCheckNearTrackers = 0.7f;
	DistanceCheckNearTrackers  = 300.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATracker::BeginPlay()
{
	Super::BeginPlay();

	SetCanAffectNavigationGeneration(false);

	if (Role == ROLE_Authority)
	{
		// Find initial point to move to.
		NextPoint = GetNextPathPoint();
	}

	GetWorld()->GetTimerManager().SetTimer(TimerNearbyTrackers, this, &ATracker::CheckNearTrackers, FrequencyCheckNearTrackers, true, 1.0f);
}

const FVector ATracker::GetNextPathPoint()
{
	// Hack to get player location
	ACharacter* Character= UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), Character);

	if (IsValid(NavPath))
		return FVector::ZeroVector;

	// The first point in the path is always the current location, we want the second one to know where to push
	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ATracker::OnHealthChanged(UCoopHealthComponent* MyHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsValid(MaterialInstance))
	{
		MaterialInstance = MeshComponent->CreateDynamicMaterialInstance(0, MeshComponent->GetMaterial(0));
	}

	if (IsValid(MaterialInstance))
		MaterialInstance->SetScalarParameterValue("LastTimeDamageReceived", GetWorld()->TimeSeconds);

	if (bDebugTracker)
		UE_LOG(LogTemp, Warning, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *(DamageCauser->GetName()));

	if (Health <= 0)
		Explode();
}

void ATracker::Explode()
{
	if (bExploded)
		return;

	bExploded = true;

	if (ExplosionEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	if (ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	MeshComponent->SetVisibility(false,true);
	//MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Damage, radial force and destruction of the tracker should only happen on server. The visual effects above are okay in clients as well.
	if (Role== ROLE_Authority)
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), PowerLevel*ExplosionDamage, GetActorLocation(), ExplosionRadius+10.0f*PowerLevel, nullptr, IgnoreActors, this, GetInstigatorController());

		if (bDebugTracker)
			DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 1);

		SetLifeSpan(2.0f);
	}
}

void ATracker::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.0f, GetInstigatorController(), this, nullptr);
}

void ATracker::CheckNearTrackers()
{
	// Hit results
	TArray<FHitResult> OutHits;

	// start and end locations
	const FVector SweepStart = GetActorLocation();
	const FVector SweepEnd = GetActorLocation();

	// create a collision sphere
	const FCollisionShape MyColSphere = FCollisionShape::MakeSphere(DistanceCheckNearTrackers);

// 	if (bDebugTracker)
// 	{
// 		// draw collision sphere
// 		DrawDebugSphere(GetWorld(), GetActorLocation(), MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);
// 	}

	TArray<AActor*> NewNearEnemies = {};

	FCollisionObjectQueryParams ObjectParams(ECC_ENEMY);

	FCollisionQueryParams TraceParams(TEXT("TraceForTrackers"), true, this); // FCollisionQueryParams
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.AddIgnoredActor(this);

	bool isHit = GetWorld()->SweepMultiByObjectType(OutHits, SweepStart, SweepEnd, FQuat::Identity, ObjectParams, MyColSphere, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			if (ATracker* Tracker = Cast<ATracker>(Hit.Actor.Get()))
				NewNearEnemies.Add(Tracker);
			//bool bIsThisTracker = Tracker ? true : false;
			//NewNearEnemies.Add(Hit.Actor);

// 			if (ATracker* Character = Cast<ATracker>(Hit.Actor))
// 				;
		}
	}

	PowerLevel = NewNearEnemies.Num();

	if (!IsValid(MaterialInstance))
	{
		MaterialInstance = MeshComponent->CreateDynamicMaterialInstance(0, MeshComponent->GetMaterial(0));
	}

	if (IsValid(MaterialInstance))
		MaterialInstance->SetScalarParameterValue("PowerLevelAlpha", PowerLevel);
}

// Called every frame
void ATracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{

		const float DistanceToTarget = (GetActorLocation() - NextPoint).Size();

		NextPoint = GetNextPathPoint();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			if (bDebugTracker != 0)
				DrawDebugString(GetWorld(), GetActorLocation(), "Target reached", (AActor*)0, FColor::Green, 0, false);
		}
		else
		{
			// Keep moving.
			FVector ForceDirection = NextPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComponent->AddImpulse(ForceDirection, NAME_None, bUseVelocityChange);

			if (bDebugTracker != 0)
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32.0f, FColor::Blue, false, 0, 0, 1);
		}

		if (bDebugTracker != 0)
			DrawDebugSphere(GetWorld(), NextPoint, 5.0f, 12, FColor::Yellow, false, 0, 0, 1);
	}
}

void ATracker::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bStartedSelfDestruct != 0 || bExploded)
		return;

	if (SelfDestructSound)
		UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);

	if (Role == ROLE_Authority)
	{
		ACoopCharacter* CharacterPawn = Cast<ACoopCharacter>(OtherActor);
		if (CharacterPawn)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerSelfDamage, this, &ATracker::DamageSelf, SelfDamageFrequency, true, 0.0f);
			bStartedSelfDestruct = true;
		}
	}
}

void ATracker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATracker, PowerLevel);
}
