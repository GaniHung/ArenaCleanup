// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterGameMode.h"
#include "Variant_Shooter/AI/ArenaWaveSpawnerManager.h"
#include "Variant_Shooter/AI/ShooterNPC.h"
#include "Variant_Shooter/ShooterHUD.h"
#include "Variant_Shooter/ShooterGameState.h"
#include "Variant_Shooter/ShooterPlayerState.h"
#include "ShooterUI.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

AShooterGameMode::AShooterGameMode()
{
	GameStateClass = AShooterGameState::StaticClass();
	PlayerStateClass = AShooterPlayerState::StaticClass();
	HUDClass = AShooterHUD::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	WaveSpawnerManagerClass = AArenaWaveSpawnerManager::StaticClass();

	static ConstructorHelpers::FClassFinder<AShooterNPC> DefaultNPCClass(TEXT("/Game/Variant_Shooter/Blueprints/AI/BP_ShooterNPC"));
	if (DefaultNPCClass.Succeeded())
	{
		WaveEnemyClass = DefaultNPCClass.Class;
	}

	WaveSpawnLocations = {
		FVector(-850.0f, -850.0f, 100.0f),
		FVector(-450.0f, -850.0f, 100.0f),
		FVector(-50.0f, -850.0f, 100.0f),
		FVector(350.0f, -850.0f, 100.0f),
		FVector(750.0f, -850.0f, 100.0f),
		FVector(-650.0f, -500.0f, 100.0f),
		FVector(650.0f, -500.0f, 100.0f)
	};
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetMatchPhase(EShooterMatchPhase::InProgress);
	}

	if (HasAuthority())
	{
		DisableLegacyWaveManagers();
		InitializeArenaWaves();
	}
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || bMatchWon)
	{
		return;
	}

	MatchElapsedSeconds += DeltaTime;
	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetMatchElapsedSeconds(MatchElapsedSeconds);
	}

	RefreshRemainingEnemies();
	CompleteWaveIfNeeded();
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte)
{
	AddTeamScore(TeamByte, 1);
}

void AShooterGameMode::AddTeamScore(uint8 TeamByte, int32 Points)
{
	if (!HasAuthority() || TeamByte != PlayerTeamByte)
	{
		return;
	}

	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->AddTeamScore(Points);
	}
}

void AShooterGameMode::OnEnemyKilled(AController* KillerController)
{
	if (!HasAuthority())
	{
		return;
	}

	AddTeamScore(PlayerTeamByte, PointsPerEnemyKill);

	if (AShooterPlayerState* KillerPS = KillerController ? KillerController->GetPlayerState<AShooterPlayerState>() : nullptr)
	{
		KillerPS->AddKill(PointsPerEnemyKill);
	}

	RefreshRemainingEnemies();
	CompleteWaveIfNeeded();
}

void AShooterGameMode::NotifyWaveAdvanced(int32 NewWaveIndex, int32 InTotalWaves, int32 InRemainingEnemies)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetWaveData(NewWaveIndex + 1, InTotalWaves);
		GS->SetRemainingEnemies(InRemainingEnemies);
	}
}

void AShooterGameMode::NotifyAllWavesComplete()
{
	if (!HasAuthority() || bMatchWon)
	{
		return;
	}

	bMatchWon = true;
	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetMatchWon(true);
		GS->SetMatchPhase(EShooterMatchPhase::Victory);
		GS->SetRemainingEnemies(0);
	}
}

void AShooterGameMode::NotifyRemainingEnemies(int32 Count)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetRemainingEnemies(Count);
	}
}

int32 AShooterGameMode::GetTeamScore(uint8 TeamByte) const
{
	if (TeamByte != PlayerTeamByte)
	{
		return 0;
	}

	if (const AShooterGameState* GS = GetShooterGameState())
	{
		return GS->GetTeamScore();
	}

	return 0;
}

AShooterGameState* AShooterGameMode::GetShooterGameState() const
{
	return GetGameState<AShooterGameState>();
}

void AShooterGameMode::InitializeArenaWaves()
{
	if (!WaveEnemyClass)
	{
		WaveEnemyClass = LoadClass<AShooterNPC>(nullptr, TEXT("/Game/Variant_Shooter/Blueprints/AI/BP_ShooterNPC.BP_ShooterNPC_C"));
	}

	if (AShooterGameState* GS = GetShooterGameState())
	{
		GS->SetWaveData(0, WaveEnemyCounts.Num());
		GS->SetRemainingEnemies(0);
		GS->SetMatchWon(false);
		GS->SetMatchPhase(EShooterMatchPhase::InProgress);
	}

	GetWorldTimerManager().SetTimer(WaveTransitionTimer, this, &AShooterGameMode::SpawnNextWave, 1.0f, false);
}

void AShooterGameMode::SpawnNextWave()
{
	if (!HasAuthority() || bMatchWon || !WaveEnemyClass)
	{
		return;
	}

	++CurrentWaveIndex;
	bWaveTransitionPending = false;
	if (!WaveEnemyCounts.IsValidIndex(CurrentWaveIndex))
	{
		NotifyAllWavesComplete();
		return;
	}

	const int32 EnemyCount = WaveEnemyCounts[CurrentWaveIndex];
	RemainingWaveEnemies = EnemyCount;
	ActiveWaveEnemies.Empty();
	NotifyWaveAdvanced(CurrentWaveIndex, WaveEnemyCounts.Num(), RemainingWaveEnemies);

	for (int32 EnemyIndex = 0; EnemyIndex < EnemyCount; ++EnemyIndex)
	{
		const FVector SpawnLocation = GetWaveSpawnLocation(EnemyIndex);
		const FRotator SpawnRotation = (FVector::ZeroVector - SpawnLocation).Rotation();
		if (APawn* SpawnedPawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), WaveEnemyClass, nullptr, SpawnLocation, SpawnRotation, true, nullptr))
		{
			if (AShooterNPC* SpawnedNPC = Cast<AShooterNPC>(SpawnedPawn))
			{
				ActiveWaveEnemies.Add(SpawnedNPC);
			}
		}
	}

	RefreshRemainingEnemies();
}

void AShooterGameMode::DisableLegacyWaveManagers()
{
	TArray<AActor*> ExistingManagers;
	UGameplayStatics::GetAllActorsOfClass(this, AArenaWaveSpawnerManager::StaticClass(), ExistingManagers);
	for (AActor* Manager : ExistingManagers)
	{
		if (IsValid(Manager))
		{
			Manager->SetActorTickEnabled(false);
			Manager->SetActorHiddenInGame(true);
			Manager->Destroy();
		}
	}
}

FVector AShooterGameMode::GetWaveSpawnLocation(int32 EnemyIndex) const
{
	if (WaveSpawnLocations.Num() == 0)
	{
		return FVector(800.0f, 0.0f, 100.0f);
	}

	const int32 SpawnIndex = (EnemyIndex + CurrentWaveIndex * 2) % WaveSpawnLocations.Num();
	const FVector Offset(0.0f, (EnemyIndex / WaveSpawnLocations.Num()) * 120.0f, 0.0f);
	return WaveSpawnLocations[SpawnIndex] + Offset;
}

int32 AShooterGameMode::RefreshRemainingEnemies()
{
	if (!HasAuthority() || bMatchWon)
	{
		return RemainingWaveEnemies;
	}

	for (int32 Index = ActiveWaveEnemies.Num() - 1; Index >= 0; --Index)
	{
		const AShooterNPC* NPC = ActiveWaveEnemies[Index].Get();
		if (!IsValid(NPC) || NPC->IsDead())
		{
			ActiveWaveEnemies.RemoveAt(Index);
		}
	}

	const int32 NewRemainingEnemies = ActiveWaveEnemies.Num();
	if (NewRemainingEnemies != RemainingWaveEnemies)
	{
		RemainingWaveEnemies = NewRemainingEnemies;
		NotifyRemainingEnemies(RemainingWaveEnemies);
	}

	return RemainingWaveEnemies;
}

void AShooterGameMode::CompleteWaveIfNeeded()
{
	if (!HasAuthority() || bMatchWon || bWaveTransitionPending || CurrentWaveIndex == INDEX_NONE || RemainingWaveEnemies > 0)
	{
		return;
	}

	bWaveTransitionPending = true;
	if (CurrentWaveIndex + 1 >= WaveEnemyCounts.Num())
	{
		NotifyAllWavesComplete();
		return;
	}

	GetWorldTimerManager().SetTimer(WaveTransitionTimer, this, &AShooterGameMode::SpawnNextWave, WaveTransitionDelay, false);
}
