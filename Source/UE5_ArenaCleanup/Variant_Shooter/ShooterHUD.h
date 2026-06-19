// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

UCLASS()
class UE5_ARENACLEANUP_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void NotifyHitBoxClick(FName BoxName) override;

private:
	void DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale = 1.0f);
	void DrawPanel(float X, float Y, float W, float H, const FLinearColor& Color);
	void DrawLine(float X1, float Y1, float X2, float Y2, const FLinearColor& Color, float Thickness = 1.0f);
	void DrawFramedPanel(float X, float Y, float W, float H, const FLinearColor& FillColor, const FLinearColor& AccentColor);
	void DrawSegmentedBar(float X, float Y, float W, float H, float Fraction, int32 Segments, const FLinearColor& FillColor, const FLinearColor& BackColor);
	void DrawAmmoPips(float X, float Y, float W, float H, int32 CurrentAmmo, int32 MagazineSize);
	void DrawButton(const FName& HitBoxName, const FString& Text, float X, float Y, float W, float H, const FLinearColor& Color);
	void DrawCrosshair();
	void DrawNetworkMenu();
};
