// Copyright Epic Games, Inc. All Rights Reserved.
// Replication patterns adapted from ReplicationGuide (MIT) GM/GS/PS model.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ShooterGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShooterMatchDataUpdatedDelegate);

UENUM(BlueprintType)
enum class EShooterMatchPhase : uint8
{
	WaitingToStart UMETA(DisplayName = "Waiting To Start"),
	InProgress UMETA(DisplayName = "In Progress"),
	Victory UMETA(DisplayName = "Victory")
};

/**
 * Replicated arena match state shared by all clients.
 */
UCLASS()
class UE5_ARENACLEANUP_API AShooterGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AShooterGameState();

	UPROPERTY(BlueprintAssignable, Category = "Arena")
	FShooterMatchDataUpdatedDelegate OnMatchDataUpdated;

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetTeamScore() const { return TeamScore; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetTotalWaves() const { return TotalWaves; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetRemainingEnemies() const { return RemainingEnemies; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	float GetMatchElapsedSeconds() const { return MatchElapsedSeconds; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	bool IsMatchWon() const { return bMatchWon; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	EShooterMatchPhase GetMatchPhase() const { return MatchPhase; }

	void SetTeamScore(int32 NewScore);
	void AddTeamScore(int32 Points);
	void SetWaveData(int32 NewCurrentWave, int32 InTotalWaves);
	void SetRemainingEnemies(int32 Count);
	void SetMatchElapsedSeconds(float Seconds);
	void SetMatchWon(bool bWon);
	void SetMatchPhase(EShooterMatchPhase NewPhase);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_TeamScore, BlueprintReadOnly, Category = "Arena")
	int32 TeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWave, BlueprintReadOnly, Category = "Arena")
	int32 CurrentWave = 0;

	UPROPERTY(ReplicatedUsing = OnRep_TotalWaves, BlueprintReadOnly, Category = "Arena")
	int32 TotalWaves = 0;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingEnemies, BlueprintReadOnly, Category = "Arena")
	int32 RemainingEnemies = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchElapsedSeconds, BlueprintReadOnly, Category = "Arena")
	float MatchElapsedSeconds = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchWon, BlueprintReadOnly, Category = "Arena")
	bool bMatchWon = false;

	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly, Category = "Arena")
	EShooterMatchPhase MatchPhase = EShooterMatchPhase::WaitingToStart;

	UFUNCTION()
	void OnRep_TeamScore();

	UFUNCTION()
	void OnRep_CurrentWave();

	UFUNCTION()
	void OnRep_TotalWaves();

	UFUNCTION()
	void OnRep_RemainingEnemies();

	UFUNCTION()
	void OnRep_MatchElapsedSeconds();

	UFUNCTION()
	void OnRep_MatchWon();

	UFUNCTION()
	void OnRep_MatchPhase();

	void BroadcastMatchDataUpdated();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
