// Copyright Epic Games, Inc. All Rights Reserved.
// Replication patterns adapted from ReplicationGuide (MIT) GM/GS/PS model.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShooterPlayerStatsUpdatedDelegate);

/**
 * Replicated per-player statistics for the arena shooter.
 */
UCLASS()
class UE5_ARENACLEANUP_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AShooterPlayerState();

	UPROPERTY(BlueprintAssignable, Category = "Arena")
	FShooterPlayerStatsUpdatedDelegate OnPlayerStatsUpdated;

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetKillCount() const { return KillCount; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetDeathCount() const { return DeathCount; }

	UFUNCTION(BlueprintPure, Category = "Arena")
	float GetReplicatedHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Arena|Ammo")
	int32 GetReplicatedMagazineSize() const { return ReplicatedMagazineSize; }

	UFUNCTION(BlueprintPure, Category = "Arena|Ammo")
	int32 GetReplicatedCurrentAmmo() const { return ReplicatedCurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Arena|Ammo")
	int32 GetReplicatedReserveAmmo() const { return ReplicatedReserveAmmo; }

	void AddKill(int32 Points = 0);
	void AddDeath();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_KillCount, BlueprintReadOnly, Category = "Arena")
	int32 KillCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_DeathCount, BlueprintReadOnly, Category = "Arena")
	int32 DeathCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedHealth, BlueprintReadOnly, Category = "Arena")
	float ReplicatedHealth = 1.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Arena|Ammo")
	int32 ReplicatedMagazineSize = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Arena|Ammo")
	int32 ReplicatedCurrentAmmo = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, BlueprintReadOnly, Category = "Arena|Ammo")
	int32 ReplicatedReserveAmmo = 0;

	UFUNCTION()
	void OnRep_KillCount();

	UFUNCTION()
	void OnRep_DeathCount();

	UFUNCTION()
	void OnRep_ReplicatedHealth();

	UFUNCTION()
	void OnRep_Ammo();

	void BroadcastStatsUpdated();

public:
	void SetReplicatedHealth(float HealthPercent);
	void SetReplicatedAmmo(int32 MagazineSize, int32 CurrentAmmo, int32 ReserveAmmo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
