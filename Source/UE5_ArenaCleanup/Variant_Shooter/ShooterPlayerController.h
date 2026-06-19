// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class UInputMappingContext;
class AShooterCharacter;
class UShooterBulletCounterUI;
class UShooterNetworkMenuWidget;
class UShooterCombatHUDWidget;
class UShooterUI;
class AShooterGameState;

/**
 *  PlayerController for the arena shooter
 *  Manages local HUD, input, server-authoritative respawn, and LAN host/join exec commands
 */
UCLASS(abstract, config="Game")
class UE5_ARENACLEANUP_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input mapping contexts for this player */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Character class to respawn when the possessed pawn is destroyed */
	UPROPERTY(EditAnywhere, Category="Shooter|Respawn")
	TSubclassOf<AShooterCharacter> CharacterClass;

	/** Type of bullet counter UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Shooter|UI")
	TSubclassOf<UShooterBulletCounterUI> BulletCounterUIClass;

	/** Type of match HUD widget to spawn on this local player */
	UPROPERTY(EditAnywhere, Category="Shooter|UI")
	TSubclassOf<UShooterUI> ShooterUIClass;

	/** Optional end-screen widget shown after the final wave */
	UPROPERTY(EditAnywhere, Category="Shooter|UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	/** Optional LAN host/join menu shown in standalone startup. */
	UPROPERTY(EditAnywhere, Category="Shooter|UI")
	TSubclassOf<UShooterNetworkMenuWidget> NetworkMenuWidgetClass;

	/** Tag to grant the possessed pawn to flag it as the player */
	UPROPERTY(EditAnywhere, Category="Shooter|Player")
	FName PlayerPawnTag = FName("Player");

	/** Pointer to the bullet counter UI widget */
	UPROPERTY()
	TObjectPtr<UShooterBulletCounterUI> BulletCounterUI;

	/** Pointer to the scoreboard / match HUD widget */
	UPROPERTY()
	TObjectPtr<UShooterUI> ShooterUI;

	UPROPERTY()
	TObjectPtr<UShooterNetworkMenuWidget> NetworkMenuWidget;

	UPROPERTY()
	TObjectPtr<UShooterCombatHUDWidget> CombatHUDWidget;

	/** Whether the victory overlay was already shown locally */
	bool bVictoryUIShown = false;

	bool bNetworkMenuVisible = false;
	bool bUseChineseUI = true;

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void OnBulletCountUpdated(int32 MagazineSize, int32 Bullets, int32 ReserveAmmo);

	UFUNCTION()
	void OnPawnDamaged(float LifePercent);

	UFUNCTION()
	void OnMatchDataUpdated();

	UFUNCTION()
	void OnPlayerStatsUpdated();

	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn();

	void BindToGameState();
	void BindToPlayerState();
	void CreateLocalHUD();
	void RefreshMatchHUD();
	void ShowNetworkMenu();
	void CloseNetworkMenu();
	bool ShouldUseTouchControls() const;

public:
	bool IsNetworkMenuVisible() const { return bNetworkMenuVisible; }
	bool IsUsingTouchControls() const { return ShouldUseTouchControls(); }
	bool IsUsingChineseUI() const { return bUseChineseUI; }
	void SetUseChineseUI(bool bInUseChinese) { bUseChineseUI = bInUseChinese; }

	void PlaySoloFromMenu();

	void JoinDefaultLanSession();

	/** Host a listen-server match on the shooter arena map (NULL subsystem / direct travel). */
	UFUNCTION(Exec)
	void HostLanSession();

	/** Join a LAN listen server at the given address (default 127.0.0.1:7777). */
	UFUNCTION(Exec)
	void JoinLanSession(const FString& Address = TEXT("127.0.0.1:7777"));
};
