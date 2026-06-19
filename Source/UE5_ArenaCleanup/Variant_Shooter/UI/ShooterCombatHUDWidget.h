// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterCombatHUDWidget.generated.h"

class UBorder;
class UCanvasPanel;
class UHorizontalBox;
class UProgressBar;
class UTextBlock;

UCLASS()
class UE5_ARENACLEANUP_API UShooterCombatHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UShooterCombatHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UBorder* CreateCard(UCanvasPanel* Root, const FName& Name, const FAnchors& Anchors, const FVector2D& Alignment, const FVector2D& Position, const FVector2D& Size, const FLinearColor& AccentColor);
	UTextBlock* CreateText(const FString& Text, int32 Size, const FLinearColor& Color, bool bCentered = false) const;
	void BuildLayout(bool bTouchLayout);
	void RefreshHUD();
	void RefreshAmmoPips(int32 Ammo, int32 MagazineSize);
	FString Localized(const TCHAR* Chinese, const TCHAR* English) const;

	UPROPERTY()
	TObjectPtr<UTextBlock> WaveText;

	UPROPERTY()
	TObjectPtr<UTextBlock> EnemiesText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY()
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY()
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY()
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY()
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ReserveText;

	UPROPERTY()
	TObjectPtr<UHorizontalBox> AmmoPips;

	UPROPERTY()
	TArray<TObjectPtr<UBorder>> AmmoPipWidgets;

	UPROPERTY()
	TObjectPtr<UTextBlock> TutorialText;

	UPROPERTY()
	TObjectPtr<UBorder> TutorialCard;

	UPROPERTY()
	TObjectPtr<UBorder> VictoryOverlay;

	UPROPERTY()
	TObjectPtr<UTextBlock> VictoryTitle;

	UPROPERTY()
	TObjectPtr<UTextBlock> VictoryDetails;

	float ConstructedWorldTime = 0.0f;
	bool bIsTouchLayout = false;
};
