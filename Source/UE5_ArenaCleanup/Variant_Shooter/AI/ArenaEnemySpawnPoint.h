// Adapted from AEnemySpawnPoint (MIT, Marty Green / KamikaziUk UE5-Arena-Shooter-Sample).
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaEnemySpawnPoint.generated.h"

/** Placed marker used by AArenaWaveSpawnerManager for enemy spawn transforms. */
UCLASS()
class UE5_ARENACLEANUP_API AArenaEnemySpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AArenaEnemySpawnPoint();
};
