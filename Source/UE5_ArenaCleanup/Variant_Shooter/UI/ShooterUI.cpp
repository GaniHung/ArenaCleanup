// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterUI.h"

#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
void DrawArenaText(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FString& Text, const FLinearColor& Color, int32 Size = 24)
{
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Bold", Size);
	FSlateDrawElement::MakeText(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Position, FVector2D(900.0f, Size + 12.0f)),
		Text,
		Font,
		ESlateDrawEffect::None,
		Color);
}

void DrawArenaBox(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color)
{
	FSlateDrawElement::MakeBox(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Position, Size),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		Color);
}
}

void UShooterUI::BP_UpdateScore_Implementation(uint8 TeamByte, int32 Score)
{
	CachedScore = Score;
	InvalidateLayoutAndVolatility();
}

void UShooterUI::BP_UpdateWave_Implementation(int32 CurrentWave, int32 TotalWaves)
{
	CachedCurrentWave = CurrentWave;
	CachedTotalWaves = TotalWaves;
	InvalidateLayoutAndVolatility();
}

void UShooterUI::BP_UpdateRemainingEnemies_Implementation(int32 RemainingEnemies)
{
	CachedRemainingEnemies = RemainingEnemies;
	InvalidateLayoutAndVolatility();
}

void UShooterUI::BP_ShowStartTutorial_Implementation()
{
	bShowTutorial = true;
	InvalidateLayoutAndVolatility();
}

void UShooterUI::BP_UpdateMatchTimer_Implementation(float ElapsedSeconds)
{
	CachedElapsedSeconds = ElapsedSeconds;
	InvalidateLayoutAndVolatility();
}

void UShooterUI::BP_OnVictory_Implementation(int32 FinalScore, float MatchTimeSeconds)
{
	bShowVictory = true;
	CachedFinalScore = FinalScore;
	CachedFinalTime = MatchTimeSeconds;
	InvalidateLayoutAndVolatility();
}

int32 UShooterUI::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	const FVector2D Size = AllottedGeometry.GetLocalSize();

	DrawArenaBox(AllottedGeometry, OutDrawElements, BaseLayer + 1, FVector2D(24.0f, 24.0f), FVector2D(430.0f, 128.0f), FLinearColor(0.0f, 0.0f, 0.0f, 0.45f));
	DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, FVector2D(44.0f, 40.0f), FString::Printf(TEXT("第 %d / %d 波"), CachedCurrentWave, CachedTotalWaves), FLinearColor::White, 28);
	DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, FVector2D(44.0f, 78.0f), FString::Printf(TEXT("敌情残余  %d"), CachedRemainingEnemies), FLinearColor(1.0f, 0.82f, 0.16f, 1.0f), 26);
	DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, FVector2D(44.0f, 114.0f), FString::Printf(TEXT("战果  %d    行动  %.0f 秒"), CachedScore, CachedElapsedSeconds), FLinearColor(0.75f, 0.9f, 1.0f, 1.0f), 22);

	if (bShowTutorial && !bShowVictory)
	{
		const FVector2D PanelSize(620.0f, 132.0f);
		const FVector2D PanelPos((Size.X - PanelSize.X) * 0.5f, 36.0f);
		DrawArenaBox(AllottedGeometry, OutDrawElements, BaseLayer + 1, PanelPos, PanelSize, FLinearColor(0.0f, 0.0f, 0.0f, 0.55f));
		DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, PanelPos + FVector2D(24.0f, 18.0f), TEXT("肃清行动 · 作战简报"), FLinearColor(0.2f, 0.85f, 1.0f, 1.0f), 30);
		DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, PanelPos + FVector2D(24.0f, 58.0f), TEXT("电脑：WASD 移动 ｜ 鼠标索敌 ｜ 左键开火 ｜ R 换弹 ｜ Shift 切枪"), FLinearColor::White, 20);
		DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 2, PanelPos + FVector2D(24.0f, 88.0f), TEXT("手机：左摇杆推进 ｜ 右侧拖动瞄准 ｜ 弹头键开火"), FLinearColor::White, 20);
	}

	if (bShowVictory)
	{
		DrawArenaBox(AllottedGeometry, OutDrawElements, BaseLayer + 10, FVector2D::ZeroVector, Size, FLinearColor(0.0f, 0.0f, 0.0f, 0.62f));
		DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 11, FVector2D(Size.X * 0.5f - 190.0f, Size.Y * 0.5f - 72.0f), TEXT("区域肃清"), FLinearColor(0.2f, 1.0f, 0.4f, 1.0f), 56);
		DrawArenaText(AllottedGeometry, OutDrawElements, BaseLayer + 11, FVector2D(Size.X * 0.5f - 210.0f, Size.Y * 0.5f), FString::Printf(TEXT("最终战果  %d    行动用时  %.0f 秒"), CachedFinalScore, CachedFinalTime), FLinearColor::White, 28);
	}

	return BaseLayer + 12;
}

