// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterUI.generated.h"

/**
 *  Simple scoreboard UI for a first person shooter game
 */
UCLASS()
class UE5_ARENACLEANUP_API UShooterUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Allows Blueprint to update score sub-widgets */
	UFUNCTION(BlueprintNativeEvent, Category="Shooter", meta = (DisplayName = "Update Score"))
	void BP_UpdateScore(uint8 TeamByte, int32 Score);

	/** Allows Blueprint to show the current wave progress */
	UFUNCTION(BlueprintNativeEvent, Category="Arena", meta = (DisplayName = "Update Wave"))
	void BP_UpdateWave(int32 CurrentWave, int32 TotalWaves);

	/** Allows Blueprint to show remaining enemies in the current wave */
	UFUNCTION(BlueprintNativeEvent, Category="Arena", meta = (DisplayName = "Update Remaining Enemies"))
	void BP_UpdateRemainingEnemies(int32 RemainingEnemies);

	/** Allows Blueprint to present start and controls tutorial UI */
	UFUNCTION(BlueprintNativeEvent, Category="Arena", meta = (DisplayName = "Show Start Tutorial"))
	void BP_ShowStartTutorial();

	/** Allows Blueprint to refresh the match timer */
	UFUNCTION(BlueprintNativeEvent, Category="Arena", meta = (DisplayName = "Update Match Timer"))
	void BP_UpdateMatchTimer(float ElapsedSeconds);

	/** Allows Blueprint to present the victory state */
	UFUNCTION(BlueprintNativeEvent, Category="Arena", meta = (DisplayName = "On Victory"))
	void BP_OnVictory(int32 FinalScore, float MatchTimeSeconds);

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	int32 CachedScore = 0;
	int32 CachedCurrentWave = 0;
	int32 CachedTotalWaves = 0;
	int32 CachedRemainingEnemies = 0;
	float CachedElapsedSeconds = 0.0f;
	bool bShowTutorial = false;
	bool bShowVictory = false;
	int32 CachedFinalScore = 0;
	float CachedFinalTime = 0.0f;
};
