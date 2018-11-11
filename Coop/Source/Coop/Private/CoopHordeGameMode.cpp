// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHordeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Coop/Public/Components/CoopHealthComponent.h"
#include "TimerManager.h"


ACoopHordeGameMode::ACoopHordeGameMode()
{
	TimeBettwenWaves = 10.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

}

void ACoopHordeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForWaveState();
}

void ACoopHordeGameMode::StartPlay()
{
	Super::StartPlay();

	if (BotClass)
		PrepareForNextWave();
}

void ACoopHordeGameMode::TrySpawnBot()
{
	SpawnBot();

	BotsToSpawn--;

	if (BotsToSpawn <= 0)
		EndWave();
}

void ACoopHordeGameMode::StartWave()
{
	UE_LOG(LogTemp, Warning, TEXT("STARTING WAVE"));

	WaveCount++;

	BotsToSpawn = 2 *WaveCount;

	GetWorldTimerManager().ClearTimer(TimerHandleStartWave);

	GetWorldTimerManager().SetTimer(TimerHandleSpawnBots, this, &ACoopHordeGameMode::TrySpawnBot, 1.0f, true, 0.0f);
}

void ACoopHordeGameMode::EndWave()
{
	UE_LOG(LogTemp, Warning, TEXT("ENDING WAVE"));

	GetWorldTimerManager().ClearTimer(TimerHandleSpawnBots);

	//PrepareForNextWave();
}

void ACoopHordeGameMode::PrepareForNextWave()
{
	UE_LOG(LogTemp, Warning, TEXT("NEXT WAVE IN %f"), TimeBettwenWaves);

	GetWorldTimerManager().SetTimer(TimerHandleStartWave, this, &ACoopHordeGameMode::StartWave, TimeBettwenWaves, false);
}

void ACoopHordeGameMode::CheckForWaveState()
{
	bool bIsPreparingNextWave = GetWorldTimerManager().IsTimerActive(TimerHandleStartWave);

	if (BotsToSpawn > 0 || bIsPreparingNextWave)
		return;

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled() || !TestPawn->IsA(BotClass))
		{
			continue;
		}

		UCoopHealthComponent* HealthComp = Cast<UCoopHealthComponent>(TestPawn->GetComponentByClass(UCoopHealthComponent::StaticClass()));

		if (HealthComp && !HealthComp->IsDead())
		{
			bIsAnyBotAlive= true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		PrepareForNextWave();
	}

	// Forms of obtaining stuff, study and consider the best

	/**
	Use an TActorIterator, Mig style
	for (TActorIterator<ATracker> ActorIt(GetWorld()); ActorIt; ++ActorIt)
	{
		if (ATracker* Bot = *ActorIt)
		{
		;
		}
	}

	Gameplaystatics style

	#include "Kismet/GameplayStatics.h"

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BotClass, FoundActors);
	for (auto Object : FoundActors)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Bot found!"));
		DrawDebugSphere(GetWorld(), Object->GetActorLocation(), 64, 32, FColor::Blue, true);
	}

	*/
}
