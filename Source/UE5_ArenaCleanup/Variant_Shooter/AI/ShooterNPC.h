// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UE5_ArenaCleanupCharacter.h"
#include "ShooterWeaponHolder.h"
#include "ShooterNPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnDeathDelegate);

class AShooterWeapon;

/**
 *  A simple AI-controlled shooter game NPC
 *  Executes its behavior through a StateTree managed by its AI Controller
 *  Holds and manages a weapon
 */
UCLASS(abstract)
class UE5_ARENACLEANUP_API AShooterNPC : public AUE5_ArenaCleanupCharacter, public IShooterWeaponHolder
{
	GENERATED_BODY()

public:

	/** Constructor */
	AShooterNPC()
	{
		bReplicates = true;
		PrimaryActorTick.bCanEverTick = true;
	}

	/** Current HP for this character. It dies if it reaches zero through damage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHP, Category="Damage")
	float CurrentHP = 100.0f;

protected:

	/** Name of the collision profile to use during ragdoll death */
	UPROPERTY(EditAnywhere, Category="Damage")
	FName RagdollCollisionProfile = FName("Ragdoll");

	/** Time to wait after death before destroying this actor */
	UPROPERTY(EditAnywhere, Category="Damage")
	float DeferredDestructionTime = 5.0f;

	/** Team byte for this character */
	UPROPERTY(EditAnywhere, Category="Team")
	uint8 TeamByte = 1;

	/** Actor tag to grant this character when it dies */
	UPROPERTY(EditAnywhere, Category="Team")
	FName DeathTag = FName("Dead");

	/** Pointer to the equipped weapon */
	TObjectPtr<AShooterWeapon> Weapon;

	/** Type of weapon to spawn for this character */
	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<AShooterWeapon> WeaponClass;

	/** Name of the first person mesh weapon socket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Weapons")
	FName FirstPersonWeaponSocket = FName("HandGrip_R");

	/** Name of the third person mesh weapon socket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Weapons")
	FName ThirdPersonWeaponSocket = FName("HandGrip_R");

	/** Max range for aiming calculations */
	UPROPERTY(EditAnywhere, Category="Aim")
	float AimRange = 10000.0f;

	/** Cone variance to apply while aiming */
	UPROPERTY(EditAnywhere, Category="Aim")
	float AimVarianceHalfAngle = 10.0f;

	/** Minimum vertical offset from the target center to apply when aiming */
	UPROPERTY(EditAnywhere, Category="Aim")
	float MinAimOffsetZ = -35.0f;

	/** Maximum vertical offset from the target center to apply when aiming */
	UPROPERTY(EditAnywhere, Category="Aim")
	float MaxAimOffsetZ = -60.0f;

	/** Actor currently being targeted */
	TObjectPtr<AActor> CurrentAimTarget;

	TObjectPtr<AActor> CombatTarget;

	/** If true, this character is currently shooting its weapon */
	bool bIsShooting = false;

	bool bAttackBurstActive = false;

	/** If true, this character has already died */
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category="Damage")
	bool bIsDead = false;

	/** Deferred destruction on death timer */
	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float PreferredCombatDistance = 900.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float FireDistance = 4500.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float FallbackMoveScale = 1.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float CloseCombatDistance = 520.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float StrafeSpreadRadius = 520.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float SeparationRadius = 260.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float SeparationStrength = 1.25f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float MoveAcceptanceRadius = 90.0f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float MoveRequestInterval = 0.35f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float DetectionDelay = 1.35f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float AttackBurstMinSeconds = 0.45f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float AttackBurstMaxSeconds = 0.85f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float RepositionMinSeconds = 1.15f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float RepositionMaxSeconds = 1.85f;

	UPROPERTY(EditAnywhere, Category="Fallback AI")
	float RepositionStrafeStrength = 360.0f;

	float LastMoveRequestTime = -100.0f;

	float TargetDetectionStartTime = -1.0f;

	float NextCombatStateChangeTime = 0.0f;

public:

	/** Delegate called when this NPC dies */
	FPawnDeathDelegate OnPawnDeath;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Gameplay cleanup */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	/** Handle incoming damage */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:

	//~Begin IShooterWeaponHolder interface

	/** Attaches a weapon's meshes to the owner */
	virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) override;

	/** Plays the firing montage for the weapon */
	virtual void PlayFiringMontage(UAnimMontage* Montage) override;

	/** Applies weapon recoil to the owner */
	virtual void AddWeaponRecoil(float Recoil) override;

	/** Updates the weapon's HUD with the current ammo count */
	virtual void UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize, int32 ReserveAmmo) override;

	/** Calculates and returns the aim location for the weapon */
	virtual FVector GetWeaponTargetLocation() override;

	/** Gives a weapon of this class to the owner */
	virtual void AddWeaponClass(const TSubclassOf<AShooterWeapon>& WeaponClass) override;

	/** Activates the passed weapon */
	virtual void OnWeaponActivated(AShooterWeapon* Weapon) override;

	/** Deactivates the passed weapon */
	virtual void OnWeaponDeactivated(AShooterWeapon* Weapon) override;

	/** Notifies the owner that the weapon cooldown has expired and it's ready to shoot again */
	virtual void OnSemiWeaponRefire() override;

	//~End IShooterWeaponHolder interface

protected:

	/** Called when HP is depleted and the character should die */
	void Die(AController* KillerController = nullptr);

	/** Called after death to destroy the actor */
	void DeferredDestruction();

	UFUNCTION()
	void OnRep_CurrentHP();

	UFUNCTION()
	void OnRep_IsDead();

	void ApplyDeathVisuals();

	void HideDebugTextComponents();

	AActor* FindFallbackTarget() const;

	bool HasLineOfSightToFallbackTarget(AActor* Target) const;

	FVector GetFallbackDesiredLocation(AActor* Target) const;

	FVector GetSeparationDirection() const;

	void ResetCombatTiming(AActor* NewTarget);

	void UpdateCombatTiming(float CurrentTime);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	/** Signals this character to start shooting at the passed actor */
	void StartShooting(AActor* ActorToShoot);

	/** Signals this character to stop shooting */
	void StopShooting();

	/** Returns true after Die() has been called */
	bool IsDead() const { return bIsDead; }
};
