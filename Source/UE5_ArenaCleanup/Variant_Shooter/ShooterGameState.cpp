// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/ShooterGameState.h"
#include "Net/UnrealNetwork.h"

AShooterGameState::AShooterGameState()
{
	bReplicates = true;
}

void AShooterGameState::SetTeamScore(int32 NewScore)
{
	if (!HasAuthority())
	{
		return;
	}

	TeamScore = NewScore;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::AddTeamScore(int32 Points)
{
	if (!HasAuthority())
	{
		return;
	}

	TeamScore += Points;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::SetWaveData(int32 NewCurrentWave, int32 InTotalWaves)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentWave = NewCurrentWave;
	TotalWaves = InTotalWaves;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::SetRemainingEnemies(int32 Count)
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingEnemies = Count;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::SetMatchElapsedSeconds(float Seconds)
{
	if (!HasAuthority())
	{
		return;
	}

	MatchElapsedSeconds = Seconds;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::SetMatchWon(bool bWon)
{
	if (!HasAuthority())
	{
		return;
	}

	bMatchWon = bWon;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::SetMatchPhase(EShooterMatchPhase NewPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	MatchPhase = NewPhase;
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_TeamScore()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_CurrentWave()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_TotalWaves()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_RemainingEnemies()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_MatchElapsedSeconds()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_MatchWon()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::OnRep_MatchPhase()
{
	BroadcastMatchDataUpdated();
}

void AShooterGameState::BroadcastMatchDataUpdated()
{
	OnMatchDataUpdated.Broadcast();
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, TeamScore);
	DOREPLIFETIME(AShooterGameState, CurrentWave);
	DOREPLIFETIME(AShooterGameState, TotalWaves);
	DOREPLIFETIME(AShooterGameState, RemainingEnemies);
	DOREPLIFETIME(AShooterGameState, MatchElapsedSeconds);
	DOREPLIFETIME(AShooterGameState, bMatchWon);
	DOREPLIFETIME(AShooterGameState, MatchPhase);
}
