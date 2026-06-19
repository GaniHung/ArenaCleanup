// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class UShooterUI;
class UUserWidget;
class AShooterGameState;
class AArenaWaveSpawnerManager;
class AShooterNPC;

/**
 *  Simple GameMode for a first person shooter game
 *  Server-only rules: scoring, waves, victory, respawn
 */
UCLASS(abstract)
class UE5_ARENACLEANUP_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterGameMode();

protected:

	/** Type of UI widget to spawn on each local player controller */
	UPROPERTY(EditAnywhere, Category="Shooter")
	TSubclassOf<UShooterUI> ShooterUIClass;

	/** Optional end-screen widget shown after the final wave */
	UPROPERTY(EditAnywhere, Category="Arena")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	/** Points awarded to the player team for each enemy kill */
	UPROPERTY(EditDefaultsOnly, Category="Arena")
	int32 PointsPerEnemyKill = 100;

	/** Team index used for the player score tally */
	UPROPERTY(EditDefaultsOnly, Category="Arena")
	uint8 PlayerTeamByte = 0;

	/** Deprecated fallback class retained for old Blueprint defaults; GameMode now owns wave spawning. */
	UPROPERTY(EditDefaultsOnly, Category="Arena")
	TSubclassOf<AArenaWaveSpawnerManager> WaveSpawnerManagerClass;

	/** Enemy class spawned by the authoritative GameMode wave system. */
	UPROPERTY(EditDefaultsOnly, Category="Arena|Waves")
	TSubclassOf<AShooterNPC> WaveEnemyClass;

	/** Required wave counts for the assignment. */
	UPROPERTY(EditDefaultsOnly, Category="Arena|Waves")
	TArray<int32> WaveEnemyCounts = {3, 5, 7};

	/** Runtime fallback spawn locations, spread around the arena. */
	UPROPERTY(EditDefaultsOnly, Category="Arena|Waves")
	TArray<FVector> WaveSpawnLocations;

	/** Seconds between clearing a wave and spawning the next one. */
	UPROPERTY(EditDefaultsOnly, Category="Arena|Waves")
	float WaveTransitionDelay = 2.0f;

	float MatchElapsedSeconds = 0.0f;
	bool bMatchWon = false;
	int32 CurrentWaveIndex = INDEX_NONE;
	int32 RemainingWaveEnemies = 0;
	FTimerHandle WaveTransitionTimer;
	TArray<TObjectPtr<AShooterNPC>> ActiveWaveEnemies;
	bool bWaveTransitionPending = false;

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	TSubclassOf<UShooterUI> GetShooterUIClass() const { return ShooterUIClass; }
	TSubclassOf<UUserWidget> GetVictoryWidgetClass() const { return VictoryWidgetClass; }

	/** Increases the score for the given team */
	void IncrementTeamScore(uint8 TeamByte);

	/** Adds points to a team score and refreshes replicated match state */
	void AddTeamScore(uint8 TeamByte, int32 Points);

	/** Called when an enemy NPC is killed */
	void OnEnemyKilled(AController* KillerController = nullptr);

	/** Called by the wave spawner when a new wave begins */
	void NotifyWaveAdvanced(int32 NewWaveIndex, int32 InTotalWaves, int32 RemainingEnemies);

	/** Called when all configured waves are cleared */
	void NotifyAllWavesComplete();

	/** Updates remaining enemy count on the replicated game state */
	void NotifyRemainingEnemies(int32 Count);

	int32 GetTeamScore(uint8 TeamByte) const;
	float GetMatchElapsedSeconds() const { return MatchElapsedSeconds; }
	bool IsMatchWon() const { return bMatchWon; }

protected:
	AShooterGameState* GetShooterGameState() const;
	void InitializeArenaWaves();
	void SpawnNextWave();
	void DisableLegacyWaveManagers();
	FVector GetWaveSpawnLocation(int32 EnemyIndex) const;
	int32 RefreshRemainingEnemies();
	void CompleteWaveIfNeeded();
};
