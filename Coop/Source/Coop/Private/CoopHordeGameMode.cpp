// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHordeGameMode.h"
#include "Coop/Public/CoopHordeGameState.h"
#include "Coop/Public/CoopPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Coop/Public/Components/CoopHealthComponent.h"
#include "TimerManager.h"

ACoopHordeGameMode::ACoopHordeGameMode()
{
	PlayerStateClass = ACoopPlayerState::StaticClass();
	GameStateClass = ACoopHordeGameState::StaticClass();

	TimeBettwenWaves = 10.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

}

void ACoopHordeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForWaveState();

	CheckForAlivePlayers();
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
	SetHordeState(EHordeState::WaveInProgress);

	WaveCount++;

	BotsToSpawn = 2 *WaveCount;

	GetWorldTimerManager().ClearTimer(TimerHandleStartWave);

	GetWorldTimerManager().SetTimer(TimerHandleSpawnBots, this, &ACoopHordeGameMode::TrySpawnBot, 1.0f, true, 0.0f);
}

void ACoopHordeGameMode::EndWave()
{
	SetHordeState(EHordeState::WaitingToComplete);

	GetWorldTimerManager().ClearTimer(TimerHandleSpawnBots);
}

void ACoopHordeGameMode::PrepareForNextWave()
{
	SetHordeState(EHordeState::WaitingToStart);

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
		SetHordeState(EHordeState::WaveComplete);
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

void ACoopHordeGameMode::CheckForAlivePlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UCoopHealthComponent* HealthComponent = Cast<UCoopHealthComponent>(MyPawn->GetComponentByClass(UCoopHealthComponent::StaticClass()));

			if (ensure(HealthComponent) && !HealthComponent->IsDead())
			{
				return;
			}
		}
	}

	// No one alive !
	GameOver();

}

void ACoopHordeGameMode::GameOver()
{
	// If the game ended, lets end any on going wave
	EndWave();

	SetHordeState(EHordeState::GameOver);
}

void ACoopHordeGameMode::SetHordeState(EHordeState HordeState)
{
	ACoopHordeGameState* GS = GetGameState<ACoopHordeGameState>();

	if (ensureAlways(GS))
	{
		GS->SetHordeState(HordeState);
	}
}
