// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterPlayerController.h"
#include "Variant_Shooter/ShooterGameMode.h"
#include "Variant_Shooter/ShooterGameState.h"
#include "Variant_Shooter/ShooterPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterCharacter.h"
#include "ShooterBulletCounterUI.h"
#include "ShooterCombatHUDWidget.h"
#include "ShooterHUD.h"
#include "ShooterMobileControlsWidget.h"
#include "ShooterNetworkMenuWidget.h"
#include "ShooterUI.h"
#include "UE5_ArenaCleanup.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (!Cast<AShooterHUD>(GetHUD()))
		{
			ClientSetHUD(AShooterHUD::StaticClass());
		}

		BindToGameState();
		BindToPlayerState();
		CombatHUDWidget = CreateWidget<UShooterCombatHUDWidget>(this, UShooterCombatHUDWidget::StaticClass());
		if (CombatHUDWidget)
		{
			CombatHUDWidget->AddToPlayerScreen(100);
		}

		if (ShouldUseTouchControls())
		{
			bEnableTouchEvents = true;
			bEnableClickEvents = true;
			ActivateTouchInterface(nullptr);

			TSubclassOf<UUserWidget> TouchWidgetClass = MobileControlsWidgetClass;
			if (!TouchWidgetClass)
			{
				TouchWidgetClass = UShooterMobileControlsWidget::StaticClass();
			}
			MobileControlsWidget = CreateWidget<UUserWidget>(this, TouchWidgetClass);
			if (MobileControlsWidget)
			{
				MobileControlsWidget->AddToPlayerScreen(200);
			}
			else
			{
				UE_LOG(LogUE5_ArenaCleanup, Error, TEXT("Could not spawn mobile controls widget."));
			}
		}

		BulletCounterUI = nullptr;

		if (GetNetMode() == NM_Standalone)
		{
			ShowNetworkMenu();
		}
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InPawn->OnDestroyed.AddDynamic(this, &AShooterPlayerController::OnPawnDestroyed);

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
	{
		ShooterCharacter->Tags.Add(PlayerPawnTag);
		ShooterCharacter->OnBulletCountUpdated.AddDynamic(this, &AShooterPlayerController::OnBulletCountUpdated);
		ShooterCharacter->OnDamaged.AddDynamic(this, &AShooterPlayerController::OnPawnDamaged);
		ShooterCharacter->OnDamaged.Broadcast(1.0f);
	}

	BindToPlayerState();
}

void AShooterPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindToPlayerState();
	OnPlayerStatsUpdated();
}

void AShooterPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	if (IsValid(BulletCounterUI))
	{
		BulletCounterUI->BP_UpdateBulletCounter(0, 0);
		BulletCounterUI->BP_UpdateAmmoCounter(0, 0, 0);
	}

	if (HasAuthority())
	{
		if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
		{
			GM->RestartPlayer(this);
		}
	}
	else
	{
		ServerRequestRespawn();
	}
}

void AShooterPlayerController::ServerRequestRespawn_Implementation()
{
	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
		GM->RestartPlayer(this);
	}
}

void AShooterPlayerController::OnBulletCountUpdated(int32 MagazineSize, int32 Bullets, int32 ReserveAmmo)
{
	if (BulletCounterUI)
	{
		BulletCounterUI->BP_UpdateBulletCounter(MagazineSize, Bullets);
		BulletCounterUI->BP_UpdateAmmoCounter(MagazineSize, Bullets, ReserveAmmo);
	}
}

void AShooterPlayerController::OnPawnDamaged(float LifePercent)
{
	if (IsValid(BulletCounterUI))
	{
		BulletCounterUI->BP_Damaged(LifePercent);
	}
}

void AShooterPlayerController::OnMatchDataUpdated()
{
	RefreshMatchHUD();
}

void AShooterPlayerController::OnPlayerStatsUpdated()
{
	if (!IsLocalPlayerController() || !BulletCounterUI)
	{
		return;
	}

	if (const AShooterPlayerState* ShooterPS = GetPlayerState<AShooterPlayerState>())
	{
		BulletCounterUI->BP_UpdateBulletCounter(ShooterPS->GetReplicatedMagazineSize(), ShooterPS->GetReplicatedCurrentAmmo());
		BulletCounterUI->BP_UpdateAmmoCounter(ShooterPS->GetReplicatedMagazineSize(), ShooterPS->GetReplicatedCurrentAmmo(), ShooterPS->GetReplicatedReserveAmmo());
	}
}

void AShooterPlayerController::BindToGameState()
{
	if (AShooterGameState* GS = GetWorld()->GetGameState<AShooterGameState>())
	{
		GS->OnMatchDataUpdated.RemoveDynamic(this, &AShooterPlayerController::OnMatchDataUpdated);
		GS->OnMatchDataUpdated.AddDynamic(this, &AShooterPlayerController::OnMatchDataUpdated);
	}
}

void AShooterPlayerController::BindToPlayerState()
{
	if (AShooterPlayerState* ShooterPS = GetPlayerState<AShooterPlayerState>())
	{
		ShooterPS->OnPlayerStatsUpdated.RemoveDynamic(this, &AShooterPlayerController::OnPlayerStatsUpdated);
		ShooterPS->OnPlayerStatsUpdated.AddDynamic(this, &AShooterPlayerController::OnPlayerStatsUpdated);
	}
}

void AShooterPlayerController::CreateLocalHUD()
{
	if (ShooterUI)
	{
		return;
	}

	TSubclassOf<UShooterUI> UIClass = ShooterUIClass;
	if (const AShooterGameMode* GM = GetWorld()->GetAuthGameMode<AShooterGameMode>())
	{
		UIClass = GM->GetShooterUIClass();
	}

	if (!UIClass)
	{
		UIClass = UShooterUI::StaticClass();
	}

	ShooterUI = CreateWidget<UShooterUI>(this, UIClass);
	if (ShooterUI)
	{
		ShooterUI->AddToViewport(0);
		ShooterUI->BP_ShowStartTutorial();
	}
}

void AShooterPlayerController::RefreshMatchHUD()
{
	if (!IsLocalPlayerController() || !ShooterUI)
	{
		return;
	}

	const AShooterGameState* GS = GetWorld()->GetGameState<AShooterGameState>();
	if (!GS)
	{
		return;
	}

	ShooterUI->BP_UpdateScore(0, GS->GetTeamScore());
	ShooterUI->BP_UpdateWave(GS->GetCurrentWave(), GS->GetTotalWaves());
	ShooterUI->BP_UpdateRemainingEnemies(GS->GetRemainingEnemies());
	ShooterUI->BP_UpdateMatchTimer(GS->GetMatchElapsedSeconds());

	if (GS->IsMatchWon() && !bVictoryUIShown)
	{
		bVictoryUIShown = true;
		ShooterUI->BP_OnVictory(GS->GetTeamScore(), GS->GetMatchElapsedSeconds());

		TSubclassOf<UUserWidget> VictoryClass = VictoryWidgetClass;
		if (const AShooterGameMode* GM = GetWorld()->GetAuthGameMode<AShooterGameMode>())
		{
			VictoryClass = GM->GetVictoryWidgetClass();
		}

		if (VictoryClass)
		{
			if (UUserWidget* VictoryWidget = CreateWidget<UUserWidget>(this, VictoryClass))
			{
				VictoryWidget->AddToViewport(100);
			}
		}
	}
}

void AShooterPlayerController::ShowNetworkMenu()
{
	if (!IsLocalPlayerController() || bNetworkMenuVisible)
	{
		return;
	}

	bNetworkMenuVisible = true;
	TSubclassOf<UShooterNetworkMenuWidget> MenuClass = NetworkMenuWidgetClass;
	if (!MenuClass)
	{
		MenuClass = UShooterNetworkMenuWidget::StaticClass();
	}
	NetworkMenuWidget = CreateWidget<UShooterNetworkMenuWidget>(this, MenuClass);
	if (NetworkMenuWidget)
	{
		NetworkMenuWidget->AddToViewport(1000);
	}
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	SetInputMode(FInputModeGameAndUI());
	SetShowMouseCursor(true);
}

void AShooterPlayerController::CloseNetworkMenu()
{
	bNetworkMenuVisible = false;
	if (NetworkMenuWidget)
	{
		NetworkMenuWidget->RemoveFromParent();
		NetworkMenuWidget = nullptr;
	}

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}

void AShooterPlayerController::PlaySoloFromMenu()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	CloseNetworkMenu();
}

void AShooterPlayerController::HostLanSession()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	CloseNetworkMenu();
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Variant_Shooter/Lvl_Shooter")), true, TEXT("listen"));
}

void AShooterPlayerController::JoinDefaultLanSession()
{
	JoinLanSession(TEXT("127.0.0.1:7777"));
}

void AShooterPlayerController::JoinLanSession(const FString& Address)
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	CloseNetworkMenu();
	ClientTravel(Address, TRAVEL_Absolute);
}

bool AShooterPlayerController::ShouldUseTouchControls() const
{
#if PLATFORM_ANDROID || PLATFORM_IOS
	return true;
#else
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
#endif
}
