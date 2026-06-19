// Adapted from AEnemySpawnerManager (MIT, Marty Green / KamikaziUk UE5-Arena-Shooter-Sample).
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/AI/ArenaWaveSpawnerManager.h"
#include "Variant_Shooter/ShooterGameMode.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
const TArray<FVector>& GetDefaultArenaSpawnLocations()
{
	static const TArray<FVector> Locations = {
		FVector(-1180.0f, -620.0f, 40.0f),
		FVector(-980.0f, 760.0f, 40.0f),
		FVector(-240.0f, 1320.0f, 40.0f),
		FVector(640.0f, 1180.0f, 40.0f),
		FVector(1120.0f, -520.0f, 40.0f),
		FVector(240.0f, -980.0f, 40.0f),
		FVector(-620.0f, -1040.0f, 40.0f)
	};
	return Locations;
}
}

AArenaWaveSpawnerManager::AArenaWaveSpawnerManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	EnemiesPerWave = {3, 5, 7};

	static ConstructorHelpers::FClassFinder<AShooterNPC> DefaultNPCClass(TEXT("/Game/Variant_Shooter/Blueprints/AI/BP_ShooterNPC"));
	if (DefaultNPCClass.Succeeded())
	{
		DefaultEnemyClass = DefaultNPCClass.Class;
	}
}

void AArenaWaveSpawnerManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (!bEnabled)
	{
		return;
	}

	BuildDefaultWavesIfNeeded();
	SpawnWave();
	if (SpawnWaves.Num() > 0)
	{
		NotifyWaveAdvanced();
	}
}

void AArenaWaveSpawnerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEnabled || !HasAuthority() || bVictoryTriggered || CurrentWave >= SpawnWaves.Num())
	{
		return;
	}

	if (IsCurrentWaveCleared())
	{
		CurrentWave += 1;
		if (CurrentWave < SpawnWaves.Num())
		{
			NotifyWaveAdvanced();
			SpawnWave();
		}
		else
		{
			NotifyMatchVictory();
		}
	}
}

void AArenaWaveSpawnerManager::BuildDefaultWavesIfNeeded()
{
	if (!DefaultEnemyClass)
	{
		DefaultEnemyClass = LoadClass<AShooterNPC>(nullptr, TEXT("/Game/Variant_Shooter/Blueprints/AI/BP_ShooterNPC.BP_ShooterNPC_C"));
	}

	bool bHasUsableSpawnPoints = false;
	for (AArenaEnemySpawnPoint* SpawnPoint : SharedSpawnPoints)
	{
		if (IsValid(SpawnPoint) && !SpawnPoint->GetActorLocation().IsNearlyZero(20.0f))
		{
			bHasUsableSpawnPoints = true;
			break;
		}
	}

	if (!bHasUsableSpawnPoints)
	{
		SharedSpawnPoints.Empty();
		for (const FVector& Location : GetDefaultArenaSpawnLocations())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AArenaEnemySpawnPoint* RuntimeSpawnPoint = GetWorld()->SpawnActor<AArenaEnemySpawnPoint>(AArenaEnemySpawnPoint::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
			if (RuntimeSpawnPoint)
			{
				SharedSpawnPoints.Add(RuntimeSpawnPoint);
			}
		}
	}

	if (SpawnWaves.Num() > 0 || EnemiesPerWave.Num() == 0 || SharedSpawnPoints.Num() == 0 || !DefaultEnemyClass)
	{
		return;
	}

	for (const int32 EnemyCount : EnemiesPerWave)
	{
		FArenaWaveData WaveData;
		for (int32 Index = 0; Index < EnemyCount; ++Index)
		{
			FArenaSpawnEntry Entry;
			Entry.SpawnPoint = SharedSpawnPoints[Index % SharedSpawnPoints.Num()];
			Entry.SpawnCharacter = DefaultEnemyClass;
			WaveData.SpawnEntries.Add(Entry);
		}
		SpawnWaves.Add(WaveData);
	}
}

void AArenaWaveSpawnerManager::SpawnWave()
{
	if (!HasAuthority() || !GetWorld() || CurrentWave >= SpawnWaves.Num())
	{
		return;
	}

	CurrentWaveCharacters.Empty();
	const TArray<FArenaSpawnEntry>& Entries = SpawnWaves[CurrentWave].SpawnEntries;

	for (const FArenaSpawnEntry& Entry : Entries)
	{
		if (!IsValid(Entry.SpawnPoint) || !Entry.SpawnCharacter)
		{
			continue;
		}

		const FRotator SpawnRotation = Entry.SpawnPoint->GetActorRotation();
		const FVector SpawnLocation = Entry.SpawnPoint->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* SpawnedPawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(
			GetWorld(),
			Entry.SpawnCharacter,
			nullptr,
			SpawnLocation,
			SpawnRotation,
			true,
			nullptr);

		if (AShooterNPC* NPC = Cast<AShooterNPC>(SpawnedPawn))
		{
			NPC->OnPawnDeath.AddDynamic(this, &AArenaWaveSpawnerManager::OnSpawnedEnemyDied);
			CurrentWaveCharacters.Add(NPC);
		}
	}

	if (AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->NotifyRemainingEnemies(CurrentWaveCharacters.Num());
	}
}

bool AArenaWaveSpawnerManager::IsCurrentWaveCleared() const
{
	if (CurrentWaveCharacters.Num() == 0)
	{
		return SpawnWaves.IsValidIndex(CurrentWave);
	}

	for (const AShooterNPC* NPC : CurrentWaveCharacters)
	{
		if (IsValid(NPC) && !NPC->IsDead())
		{
			return false;
		}
	}

	return true;
}

void AArenaWaveSpawnerManager::OnSpawnedEnemyDied()
{
	UpdateEnemies();
}

void AArenaWaveSpawnerManager::UpdateEnemies()
{
	for (int32 Index = CurrentWaveCharacters.Num() - 1; Index >= 0; --Index)
	{
		const AShooterNPC* NPC = CurrentWaveCharacters[Index].Get();
		if (!IsValid(NPC) || NPC->IsDead())
		{
			CurrentWaveCharacters.RemoveAt(Index);
		}
	}

	if (HasAuthority())
	{
		if (AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->NotifyRemainingEnemies(CurrentWaveCharacters.Num());
		}
	}
}

TArray<AShooterNPC*> AArenaWaveSpawnerManager::GetWaveCharacters()
{
	UpdateEnemies();
	TArray<AShooterNPC*> Result;
	for (AShooterNPC* NPC : CurrentWaveCharacters)
	{
		if (IsValid(NPC))
		{
			Result.Add(NPC);
		}
	}
	return Result;
}

void AArenaWaveSpawnerManager::NotifyWaveAdvanced()
{
	OnWaveAdvanced.Broadcast(CurrentWave, SpawnWaves.Num());
	if (AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->NotifyWaveAdvanced(CurrentWave, SpawnWaves.Num(), CurrentWaveCharacters.Num());
	}
}

void AArenaWaveSpawnerManager::NotifyMatchVictory()
{
	if (bVictoryTriggered)
	{
		return;
	}

	bVictoryTriggered = true;
	OnAllWavesComplete.Broadcast();
	if (AShooterGameMode* GameMode = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->NotifyAllWavesComplete();
	}
}
