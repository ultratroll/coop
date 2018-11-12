// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopHordeGameState.h"
#include "Net/UnrealNetwork.h"


void ACoopHordeGameState::OnRep_HordeState(EHordeState OldHordeState)
{
	OnWaveStateChanged(OldHordeState, HordeState);
}

void ACoopHordeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopHordeGameState, HordeState);
}

void ACoopHordeGameState::SetHordeState(EHordeState NewHordeState)
{
	if (HasAuthority())
	{
		EHordeState const OldHordeState = HordeState;
		HordeState = NewHordeState;
		OnRep_HordeState(OldHordeState);
	}
}
