// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterBulletCounterUI.h"

#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
void DrawShooterText(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FString& Text, const FLinearColor& Color, int32 Size = 24)
{
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Bold", Size);
	FSlateDrawElement::MakeText(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Position, FVector2D(520.0f, Size + 12.0f)),
		Text,
		Font,
		ESlateDrawEffect::None,
		Color);
}

void DrawShooterBox(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color)
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

void UShooterBulletCounterUI::BP_UpdateBulletCounter_Implementation(int32 MagazineSize, int32 BulletCount)
{
	CachedMagazineSize = MagazineSize;
	CachedBulletCount = BulletCount;
	InvalidateLayoutAndVolatility();
}

void UShooterBulletCounterUI::BP_UpdateAmmoCounter_Implementation(int32 MagazineSize, int32 BulletCount, int32 ReserveAmmo)
{
	CachedMagazineSize = MagazineSize;
	CachedBulletCount = BulletCount;
	CachedReserveAmmo = ReserveAmmo;
	InvalidateLayoutAndVolatility();
}

void UShooterBulletCounterUI::BP_Damaged_Implementation(float LifePercent)
{
	CachedLifePercent = FMath::Clamp(LifePercent, 0.0f, 1.0f);
	InvalidateLayoutAndVolatility();
}

int32 UShooterBulletCounterUI::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FVector2D PanelSize(390.0f, 112.0f);
	const FVector2D PanelPos(Size.X - PanelSize.X - 28.0f, Size.Y - PanelSize.Y - 28.0f);

	DrawShooterBox(AllottedGeometry, OutDrawElements, BaseLayer + 1, PanelPos, PanelSize, FLinearColor(0.0f, 0.0f, 0.0f, 0.48f));
	DrawShooterText(AllottedGeometry, OutDrawElements, BaseLayer + 2, PanelPos + FVector2D(20.0f, 16.0f), FString::Printf(TEXT("弹匣  %d / %d"), CachedBulletCount, CachedMagazineSize), FLinearColor::White, 30);

	const FString ReserveText = CachedReserveAmmo < 0 ? TEXT("备弹  ∞") : FString::Printf(TEXT("备弹  %d"), CachedReserveAmmo);
	DrawShooterText(AllottedGeometry, OutDrawElements, BaseLayer + 2, PanelPos + FVector2D(20.0f, 56.0f), ReserveText, FLinearColor(1.0f, 0.82f, 0.16f, 1.0f), 24);

	const FVector2D HealthPos(28.0f, Size.Y - 56.0f);
	const FVector2D HealthSize(320.0f, 24.0f);
	DrawShooterBox(AllottedGeometry, OutDrawElements, BaseLayer + 1, HealthPos, HealthSize, FLinearColor(0.05f, 0.05f, 0.05f, 0.72f));
	DrawShooterBox(AllottedGeometry, OutDrawElements, BaseLayer + 2, HealthPos, FVector2D(HealthSize.X * CachedLifePercent, HealthSize.Y), FLinearColor(0.1f, 0.95f, 0.28f, 0.9f));
	DrawShooterText(AllottedGeometry, OutDrawElements, BaseLayer + 3, HealthPos + FVector2D(0.0f, -34.0f), FString::Printf(TEXT("生命体征  %.0f%%"), CachedLifePercent * 100.0f), FLinearColor::White, 22);

	return BaseLayer + 4;
}

