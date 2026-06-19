// Adapted from AEnemySpawnerManager (MIT, Marty Green / KamikaziUk UE5-Arena-Shooter-Sample).
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaEnemySpawnPoint.h"
#include "ShooterNPC.h"
#include "ArenaWaveSpawnerManager.generated.h"

USTRUCT()
struct FArenaSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<AArenaEnemySpawnPoint> SpawnPoint = nullptr;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AShooterNPC> SpawnCharacter;
};

USTRUCT()
struct FArenaWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<FArenaSpawnEntry> SpawnEntries;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FArenaWaveAdvancedDelegate, int32, NewWaveIndex, int32, TotalWaves);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FArenaAllWavesCompleteDelegate);

/**
 * Multi-wave enemy spawner adapted from the MIT Arena Shooter sample.
 * Spawns official AShooterNPC pawns and notifies the GameMode when all waves are cleared.
 */
UCLASS()
class UE5_ARENACLEANUP_API AArenaWaveSpawnerManager : public AActor
{
	GENERATED_BODY()

public:
	AArenaWaveSpawnerManager();

	UPROPERTY(EditAnywhere, Category = "Wave")
	bool bEnabled = false;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<FArenaWaveData> SpawnWaves;

	/** Optional fallback spawn markers when waves are built from EnemiesPerWave. */
	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<TObjectPtr<AArenaEnemySpawnPoint>> SharedSpawnPoints;

	/** Used when SpawnWaves is empty at BeginPlay. Default: 3, 5, 7. */
	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<int32> EnemiesPerWave;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TSubclassOf<AShooterNPC> DefaultEnemyClass;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FArenaWaveAdvancedDelegate OnWaveAdvanced;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FArenaAllWavesCompleteDelegate OnAllWavesComplete;

	int32 GetCurrentWaveIndex() const { return CurrentWave; }
	int32 GetTotalWaves() const { return SpawnWaves.Num(); }

	TArray<AShooterNPC*> GetWaveCharacters();
	void UpdateEnemies();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	void BuildDefaultWavesIfNeeded();
	void SpawnWave();
	void NotifyWaveAdvanced();
	void NotifyMatchVictory();
	bool IsCurrentWaveCleared() const;

	UFUNCTION()
	void OnSpawnedEnemyDied();

	int32 CurrentWave = 0;
	TArray<TObjectPtr<AShooterNPC>> CurrentWaveCharacters;
	bool bVictoryTriggered = false;
};
