// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/ShooterPlayerState.h"
#include "Net/UnrealNetwork.h"

AShooterPlayerState::AShooterPlayerState()
{
	bReplicates = true;
}

float AShooterPlayerState::GetReplicatedHealthPercent() const
{
	return ReplicatedHealth;
}

void AShooterPlayerState::AddKill(int32 Points)
{
	if (!HasAuthority())
	{
		return;
	}

	++KillCount;
	SetScore(GetScore() + Points);
	BroadcastStatsUpdated();
}

void AShooterPlayerState::AddDeath()
{
	if (!HasAuthority())
	{
		return;
	}

	++DeathCount;
	BroadcastStatsUpdated();
}

void AShooterPlayerState::SetReplicatedHealth(float HealthPercent)
{
	if (!HasAuthority())
	{
		return;
	}

	ReplicatedHealth = FMath::Clamp(HealthPercent, 0.0f, 1.0f);
	BroadcastStatsUpdated();
}

void AShooterPlayerState::SetReplicatedAmmo(int32 MagazineSize, int32 CurrentAmmo, int32 ReserveAmmo)
{
	if (!HasAuthority())
	{
		return;
	}

	ReplicatedMagazineSize = FMath::Max(0, MagazineSize);
	ReplicatedCurrentAmmo = FMath::Max(0, CurrentAmmo);
	ReplicatedReserveAmmo = ReserveAmmo;
	BroadcastStatsUpdated();
}

void AShooterPlayerState::OnRep_KillCount()
{
	BroadcastStatsUpdated();
}

void AShooterPlayerState::OnRep_DeathCount()
{
	BroadcastStatsUpdated();
}

void AShooterPlayerState::OnRep_ReplicatedHealth()
{
	BroadcastStatsUpdated();
}

void AShooterPlayerState::OnRep_Ammo()
{
	BroadcastStatsUpdated();
}

void AShooterPlayerState::BroadcastStatsUpdated()
{
	OnPlayerStatsUpdated.Broadcast();
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, KillCount);
	DOREPLIFETIME(AShooterPlayerState, DeathCount);
	DOREPLIFETIME(AShooterPlayerState, ReplicatedHealth);
	DOREPLIFETIME(AShooterPlayerState, ReplicatedMagazineSize);
	DOREPLIFETIME(AShooterPlayerState, ReplicatedCurrentAmmo);
	DOREPLIFETIME(AShooterPlayerState, ReplicatedReserveAmmo);
}
