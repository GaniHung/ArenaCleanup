// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/ShooterHUD.h"

#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "ShooterCharacter.h"
#include "ShooterGameState.h"
#include "ShooterPlayerState.h"
#include "ShooterPlayerController.h"
#include "ShooterWeapon.h"

namespace
{
const FName HostButtonName(TEXT("ArenaHostButton"));
const FName JoinButtonName(TEXT("ArenaJoinButton"));
const FName SoloButtonName(TEXT("ArenaSoloButton"));
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AShooterHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	AShooterPlayerController* ShooterPC = Cast<AShooterPlayerController>(PlayerOwner);
	if (!ShooterPC || !ShooterPC->IsNetworkMenuVisible())
	{
		return;
	}

	if (BoxName == HostButtonName)
	{
		ShooterPC->HostLanSession();
	}
	else if (BoxName == JoinButtonName)
	{
		ShooterPC->JoinDefaultLanSession();
	}
	else if (BoxName == SoloButtonName)
	{
		ShooterPC->PlaySoloFromMenu();
	}
}

void AShooterHUD::DrawTextLine(const FString& Text, float X, float Y, const FLinearColor& Color, float Scale)
{
	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(Text), GEngine->GetLargeFont(), Color);
	TextItem.Scale = FVector2D(Scale, Scale);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TextItem);
}

void AShooterHUD::DrawPanel(float X, float Y, float W, float H, const FLinearColor& Color)
{
	FCanvasTileItem Tile(FVector2D(X, Y), FVector2D(W, H), Color);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}

void AShooterHUD::DrawLine(float X1, float Y1, float X2, float Y2, const FLinearColor& Color, float Thickness)
{
	FCanvasLineItem Line(FVector2D(X1, Y1), FVector2D(X2, Y2));
	Line.SetColor(Color);
	Line.LineThickness = Thickness;
	Canvas->DrawItem(Line);
}

void AShooterHUD::DrawFramedPanel(float X, float Y, float W, float H, const FLinearColor& FillColor, const FLinearColor& AccentColor)
{
	DrawPanel(X, Y, W, H, FillColor);
	DrawPanel(X, Y, W, 3.0f, AccentColor);
	DrawLine(X, Y, X + W, Y, AccentColor, 1.0f);
	DrawLine(X, Y + H, X + W, Y + H, FLinearColor(1.0f, 1.0f, 1.0f, 0.12f), 1.0f);
	DrawLine(X, Y, X, Y + H, FLinearColor(1.0f, 1.0f, 1.0f, 0.1f), 1.0f);
	DrawLine(X + W, Y, X + W, Y + H, FLinearColor(1.0f, 1.0f, 1.0f, 0.1f), 1.0f);
	DrawPanel(X + 10.0f, Y + H - 2.0f, 58.0f, 2.0f, AccentColor);
}

void AShooterHUD::DrawSegmentedBar(float X, float Y, float W, float H, float Fraction, int32 Segments, const FLinearColor& FillColor, const FLinearColor& BackColor)
{
	const int32 ClampedSegments = FMath::Max(1, Segments);
	const float Gap = 3.0f;
	const float SegmentW = (W - Gap * (ClampedSegments - 1)) / ClampedSegments;
	const int32 FilledSegments = FMath::CeilToInt(FMath::Clamp(Fraction, 0.0f, 1.0f) * ClampedSegments);

	for (int32 Index = 0; Index < ClampedSegments; ++Index)
	{
		const float SegmentX = X + Index * (SegmentW + Gap);
		DrawPanel(SegmentX, Y, SegmentW, H, Index < FilledSegments ? FillColor : BackColor);
	}
}

void AShooterHUD::DrawAmmoPips(float X, float Y, float W, float H, int32 CurrentAmmo, int32 MagazineSize)
{
	const int32 PipCount = FMath::Clamp(MagazineSize > 0 ? MagazineSize : 1, 1, 30);
	const int32 FilledPips = MagazineSize > 0 ? FMath::Clamp(FMath::CeilToInt(static_cast<float>(CurrentAmmo) / MagazineSize * PipCount), 0, PipCount) : 0;
	const float Gap = 3.0f;
	const float PipW = (W - Gap * (PipCount - 1)) / PipCount;

	for (int32 Index = 0; Index < PipCount; ++Index)
	{
		const float PipX = X + Index * (PipW + Gap);
		const bool bFilled = Index < FilledPips;
		const float TipH = H * 0.26f;
		DrawPanel(PipX, Y + TipH, PipW, H - TipH, bFilled ? FLinearColor(1.0f, 0.62f, 0.18f, 0.95f) : FLinearColor(0.18f, 0.18f, 0.2f, 0.72f));
		DrawPanel(PipX + PipW * 0.22f, Y, PipW * 0.56f, TipH + 1.0f, bFilled ? FLinearColor(1.0f, 0.84f, 0.36f, 0.95f) : FLinearColor(0.11f, 0.12f, 0.14f, 0.72f));
	}
}

void AShooterHUD::DrawButton(const FName& HitBoxName, const FString& Text, float X, float Y, float W, float H, const FLinearColor& Color)
{
	DrawPanel(X, Y, W, H, Color);
	DrawPanel(X, Y, W, 5.0f, FLinearColor(0.0f, 0.85f, 1.0f, 0.95f));
	DrawTextLine(Text, X + 24.0f, Y + H * 0.5f - 15.0f, FLinearColor::White, 1.08f);
	AddHitBox(FVector2D(X, Y), FVector2D(W, H), HitBoxName, true, 50);
}

void AShooterHUD::DrawCrosshair()
{
	const FVector2D Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
	const FLinearColor CrosshairColor(0.0f, 0.92f, 1.0f, 0.88f);
	DrawLine(Center.X - 24.0f, Center.Y, Center.X - 8.0f, Center.Y, CrosshairColor, 2.0f);
	DrawLine(Center.X + 8.0f, Center.Y, Center.X + 24.0f, Center.Y, CrosshairColor, 2.0f);
	DrawLine(Center.X, Center.Y - 24.0f, Center.X, Center.Y - 8.0f, CrosshairColor, 2.0f);
	DrawLine(Center.X, Center.Y + 8.0f, Center.X, Center.Y + 24.0f, CrosshairColor, 2.0f);
	DrawPanel(Center.X - 2.0f, Center.Y - 2.0f, 4.0f, 4.0f, FLinearColor::White);
}

void AShooterHUD::DrawNetworkMenu()
{
	DrawPanel(0.0f, 0.0f, Canvas->SizeX, Canvas->SizeY, FLinearColor(0.0f, 0.0f, 0.0f, 0.42f));

	const float PanelW = FMath::Min(720.0f, Canvas->SizeX - 80.0f);
	const float PanelH = 430.0f;
	const float X = Canvas->SizeX * 0.5f - PanelW * 0.5f;
	const float Y = Canvas->SizeY * 0.5f - PanelH * 0.5f;
	DrawPanel(X, Y, PanelW, PanelH, FLinearColor(0.015f, 0.025f, 0.05f, 0.88f));
	DrawPanel(X, Y, PanelW, 8.0f, FLinearColor(0.0f, 0.85f, 1.0f, 1.0f));
	DrawTextLine(TEXT("ARENA CLEANUP"), X + 38.0f, Y + 36.0f, FLinearColor::White, 1.65f);
	DrawTextLine(TEXT("LAN CO-OP QUICK START"), X + 40.0f, Y + 90.0f, FLinearColor(0.2f, 0.85f, 1.0f, 1.0f), 0.98f);
	DrawTextLine(TEXT("Host on one device. Client joins 127.0.0.1 for local test; use console: JoinLanSession <HostIP:7777> for real LAN IP."), X + 40.0f, Y + 126.0f, FLinearColor(0.82f, 0.9f, 1.0f, 1.0f), 0.62f);

	const float ButtonW = PanelW - 80.0f;
	DrawButton(HostButtonName, TEXT("HOST LISTEN SERVER"), X + 40.0f, Y + 178.0f, ButtonW, 62.0f, FLinearColor(0.02f, 0.26f, 0.46f, 0.92f));
	DrawButton(JoinButtonName, TEXT("JOIN 127.0.0.1:7777"), X + 40.0f, Y + 256.0f, ButtonW, 62.0f, FLinearColor(0.12f, 0.16f, 0.24f, 0.92f));
	DrawButton(SoloButtonName, TEXT("PLAY SOLO / CLOSE MENU"), X + 40.0f, Y + 334.0f, ButtonW, 62.0f, FLinearColor(0.16f, 0.12f, 0.08f, 0.92f));
}
