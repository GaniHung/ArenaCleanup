// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterCombatHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "ShooterCharacter.h"
#include "ShooterGameState.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "ShooterWeapon.h"
#include "Styling/CoreStyle.h"

namespace
{
const FLinearColor CardColor(0.008f, 0.018f, 0.04f, 0.86f);
const FLinearColor Cyan(0.0f, 0.86f, 1.0f, 1.0f);
const FLinearColor Orange(1.0f, 0.50f, 0.14f, 1.0f);
const FLinearColor Yellow(1.0f, 0.83f, 0.18f, 1.0f);
const FLinearColor Muted(0.67f, 0.78f, 0.9f, 1.0f);
}

UShooterCombatHUDWidget::UShooterCombatHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetIsFocusable(false);
}

void UShooterCombatHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WidgetTree->RootWidget)
	{
		return;
	}

	const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>();
	bIsTouchLayout = ShooterPC && ShooterPC->IsUsingTouchControls();
	ConstructedWorldTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	BuildLayout(bIsTouchLayout);
	RefreshHUD();
}

void UShooterCombatHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshHUD();
}

void UShooterCombatHUDWidget::BuildLayout(bool bTouchLayout)
{
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CombatHUDRoot"));
	WidgetTree->RootWidget = Root;

	const FVector2D WavePosition = bTouchLayout ? FVector2D(30.0f, 26.0f) : FVector2D(52.0f, 48.0f);
	const FVector2D WaveSize = bTouchLayout ? FVector2D(390.0f, 146.0f) : FVector2D(500.0f, 184.0f);
	UBorder* WaveCard = CreateCard(Root, TEXT("WaveCard"), FAnchors(0.0f, 0.0f), FVector2D::ZeroVector, WavePosition, WaveSize, Cyan);
	UVerticalBox* WaveBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	WaveCard->SetContent(WaveBox);
	UTextBlock* ArenaLabel = CreateText(Localized(TEXT("作战态势"), TEXT("COMBAT STATUS")), bTouchLayout ? 18 : 21, Muted);
	WaveBox->AddChildToVerticalBox(ArenaLabel)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	WaveText = CreateText(TEXT(""), bTouchLayout ? 34 : 42, FLinearColor::White);
	WaveBox->AddChildToVerticalBox(WaveText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 5.0f));
	EnemiesText = CreateText(TEXT(""), bTouchLayout ? 25 : 29, Yellow);
	WaveBox->AddChildToVerticalBox(EnemiesText);

	const FVector2D ScoreSize = bTouchLayout ? FVector2D(400.0f, 76.0f) : FVector2D(500.0f, 86.0f);
	UBorder* ScoreCard = CreateCard(Root, TEXT("ScoreCard"), FAnchors(0.5f, 0.0f), FVector2D(0.5f, 0.0f), FVector2D(0.0f, bTouchLayout ? 26.0f : 48.0f), ScoreSize, Cyan);
	UHorizontalBox* ScoreBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ScoreCard->SetContent(ScoreBox);
	ScoreText = CreateText(TEXT(""), bTouchLayout ? 23 : 27, FLinearColor::White);
	ScoreBox->AddChildToHorizontalBox(ScoreText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TimeText = CreateText(TEXT(""), bTouchLayout ? 23 : 27, Muted, true);
	ScoreBox->AddChildToHorizontalBox(TimeText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	const FVector2D AmmoSize = bTouchLayout ? FVector2D(410.0f, 146.0f) : FVector2D(500.0f, 184.0f);
	UBorder* AmmoCard = CreateCard(Root, TEXT("AmmoCard"), FAnchors(1.0f, 0.0f), FVector2D(1.0f, 0.0f), FVector2D(bTouchLayout ? -30.0f : -52.0f, bTouchLayout ? 26.0f : 48.0f), AmmoSize, Orange);
	UVerticalBox* AmmoBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	AmmoCard->SetContent(AmmoBox);
	UTextBlock* WeaponLabel = CreateText(Localized(TEXT("武器状态"), TEXT("WEAPON STATUS")), bTouchLayout ? 18 : 21, Muted);
	AmmoBox->AddChildToVerticalBox(WeaponLabel)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 5.0f));
	UHorizontalBox* AmmoNumbers = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	AmmoBox->AddChildToVerticalBox(AmmoNumbers)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 9.0f));
	AmmoText = CreateText(TEXT(""), bTouchLayout ? 31 : 39, FLinearColor::White);
	AmmoNumbers->AddChildToHorizontalBox(AmmoText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ReserveText = CreateText(TEXT(""), bTouchLayout ? 19 : 22, Yellow, true);
	AmmoNumbers->AddChildToHorizontalBox(ReserveText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	AmmoPips = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	AmmoBox->AddChildToVerticalBox(AmmoPips)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	for (int32 Index = 0; Index < 10; ++Index)
	{
		UBorder* Pip = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		Pip->SetBrushColor(FLinearColor(0.13f, 0.15f, 0.2f, 0.92f));
		AmmoPipWidgets.Add(Pip);
		if (UHorizontalBoxSlot* PipSlot = AmmoPips->AddChildToHorizontalBox(Pip))
		{
			PipSlot->SetPadding(FMargin(2.5f, 0.0f));
			PipSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
	}

	const FAnchors HealthAnchors = bTouchLayout ? FAnchors(0.0f, 0.0f) : FAnchors(0.0f, 1.0f);
	const FVector2D HealthAlignment = bTouchLayout ? FVector2D::ZeroVector : FVector2D(0.0f, 1.0f);
	const FVector2D HealthPosition = bTouchLayout ? FVector2D(30.0f, 186.0f) : FVector2D(52.0f, -52.0f);
	const FVector2D HealthSize = bTouchLayout ? FVector2D(390.0f, 92.0f) : FVector2D(500.0f, 116.0f);
	UBorder* HealthCard = CreateCard(Root, TEXT("HealthCard"), HealthAnchors, HealthAlignment, HealthPosition, HealthSize, FLinearColor(0.12f, 0.95f, 0.45f, 1.0f));
	UVerticalBox* HealthBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	HealthCard->SetContent(HealthBox);
	HealthText = CreateText(TEXT(""), bTouchLayout ? 22 : 27, FLinearColor::White);
	HealthBox->AddChildToVerticalBox(HealthText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
	HealthBar->SetFillColorAndOpacity(FLinearColor(0.1f, 0.95f, 0.42f, 1.0f));
	HealthBox->AddChildToVerticalBox(HealthBar)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	TutorialCard = CreateCard(Root, TEXT("TutorialCard"), FAnchors(0.5f, 0.0f), FVector2D(0.5f, 0.0f), FVector2D(0.0f, bTouchLayout ? 118.0f : 154.0f), bTouchLayout ? FVector2D(820.0f, 62.0f) : FVector2D(900.0f, 76.0f), FLinearColor(1.0f, 1.0f, 1.0f, 0.24f));
	TutorialText = CreateText(TEXT(""), bTouchLayout ? 20 : 23, FLinearColor::White, true);
	TutorialText->SetAutoWrapText(true);
	TutorialCard->SetContent(TutorialText);

	UTextBlock* Crosshair = CreateText(TEXT("+"), bTouchLayout ? 34 : 42, Cyan, true);
	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(Crosshair))
	{
		CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetSize(FVector2D(72.0f, 72.0f));
	}

	VictoryOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("VictoryOverlay"));
	VictoryOverlay->SetBrushColor(FLinearColor(0.0f, 0.02f, 0.05f, 0.9f));
	VictoryOverlay->SetVisibility(ESlateVisibility::Collapsed);
	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(VictoryOverlay))
	{
		CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		CanvasSlot->SetOffsets(FMargin(0.0f));
	}
	UVerticalBox* VictoryBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	VictoryOverlay->SetContent(VictoryBox);
	VictoryTitle = CreateText(TEXT(""), 64, FLinearColor(0.18f, 1.0f, 0.52f, 1.0f), true);
	VictoryBox->AddChildToVerticalBox(VictoryTitle)->SetPadding(FMargin(0.0f, 300.0f, 0.0f, 22.0f));
	VictoryDetails = CreateText(TEXT(""), 30, FLinearColor::White, true);
	VictoryBox->AddChildToVerticalBox(VictoryDetails);
}

UBorder* UShooterCombatHUDWidget::CreateCard(UCanvasPanel* Root, const FName& Name, const FAnchors& Anchors, const FVector2D& Alignment, const FVector2D& Position, const FVector2D& Size, const FLinearColor& AccentColor)
{
	UOverlay* Frame = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	if (UCanvasPanelSlot* CanvasSlot = Root->AddChildToCanvas(Frame))
	{
		CanvasSlot->SetAnchors(Anchors);
		CanvasSlot->SetAlignment(Alignment);
		CanvasSlot->SetPosition(Position);
		CanvasSlot->SetSize(Size);
	}

	UBorder* Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
	Card->SetBrushColor(CardColor);
	Card->SetPadding(FMargin(26.0f, 20.0f));
	if (UOverlaySlot* CardSlot = Frame->AddChildToOverlay(Card))
	{
		CardSlot->SetHorizontalAlignment(HAlign_Fill);
		CardSlot->SetVerticalAlignment(VAlign_Fill);
	}

	UBorder* Accent = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Accent->SetBrushColor(AccentColor);
	if (UOverlaySlot* AccentSlot = Frame->AddChildToOverlay(Accent))
	{
		AccentSlot->SetHorizontalAlignment(HAlign_Fill);
		AccentSlot->SetVerticalAlignment(VAlign_Top);
		AccentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, Size.Y - 5.0f));
	}
	return Card;
}

UTextBlock* UShooterCombatHUDWidget::CreateText(const FString& Text, int32 Size, const FLinearColor& Color, bool bCentered) const
{
	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TextBlock->SetText(FText::FromString(Text));
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	TextBlock->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", Size));
	TextBlock->SetJustification(bCentered ? ETextJustify::Center : ETextJustify::Left);
	TextBlock->SetShadowOffset(FVector2D(1.5f, 1.5f));
	TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.86f));
	return TextBlock;
}

void UShooterCombatHUDWidget::RefreshHUD()
{
	const AShooterGameState* ShooterGS = GetWorld() ? GetWorld()->GetGameState<AShooterGameState>() : nullptr;
	const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>();
	const AShooterPlayerState* ShooterPS = ShooterPC ? ShooterPC->GetPlayerState<AShooterPlayerState>() : nullptr;
	if (!ShooterGS || !ShooterPC || !ShooterPS)
	{
		return;
	}

	int32 Magazine = ShooterPS->GetReplicatedMagazineSize();
	int32 Ammo = ShooterPS->GetReplicatedCurrentAmmo();
	int32 Reserve = ShooterPS->GetReplicatedReserveAmmo();
	if (Magazine <= 0)
	{
		if (const AShooterCharacter* Character = Cast<AShooterCharacter>(ShooterPC->GetPawn()))
		{
			if (const AShooterWeapon* Weapon = Character->GetCurrentWeapon())
			{
				Magazine = Weapon->GetMagazineSize();
				Ammo = Weapon->GetBulletCount();
				Reserve = Weapon->GetReserveAmmo();
			}
		}
	}

	const float Health = FMath::Clamp(ShooterPS->GetReplicatedHealthPercent(), 0.0f, 1.0f);
	const bool bChinese = ShooterPC->IsUsingChineseUI();
	const int32 ElapsedSeconds = FMath::Max(0, FMath::FloorToInt(ShooterGS->GetMatchElapsedSeconds()));
	const int32 ElapsedMinutes = ElapsedSeconds / 60;
	const int32 RemainingSeconds = ElapsedSeconds % 60;
	WaveText->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("第 %d / %d 波"), ShooterGS->GetCurrentWave(), ShooterGS->GetTotalWaves()) : FString::Printf(TEXT("WAVE  %d / %d"), ShooterGS->GetCurrentWave(), ShooterGS->GetTotalWaves())));
	EnemiesText->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("敌情残余  %d"), ShooterGS->GetRemainingEnemies()) : FString::Printf(TEXT("HOSTILES LEFT  %d"), ShooterGS->GetRemainingEnemies())));
	ScoreText->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("战果  %d"), ShooterGS->GetTeamScore()) : FString::Printf(TEXT("SCORE  %d"), ShooterGS->GetTeamScore())));
	TimeText->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("行动  %02d:%02d"), ElapsedMinutes, RemainingSeconds) : FString::Printf(TEXT("TIME  %02d:%02d"), ElapsedMinutes, RemainingSeconds)));
	HealthText->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("生命体征  %.0f%%"), Health * 100.0f) : FString::Printf(TEXT("VITALS  %.0f%%"), Health * 100.0f)));
	HealthBar->SetPercent(Health);
	HealthBar->SetFillColorAndOpacity(Health > 0.35f ? FLinearColor(0.1f, 0.95f, 0.42f, 1.0f) : FLinearColor(1.0f, 0.16f, 0.12f, 1.0f));
	AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%02d / %02d"), Ammo, Magazine)));
	ReserveText->SetText(FText::FromString(Reserve < 0 ? Localized(TEXT("备弹 ∞"), TEXT("RESERVE ∞")) : (bChinese ? FString::Printf(TEXT("备弹 %d"), Reserve) : FString::Printf(TEXT("RESERVE %d"), Reserve))));
	RefreshAmmoPips(Ammo, Magazine);

	const float TutorialAge = (GetWorld() ? GetWorld()->GetTimeSeconds() : ConstructedWorldTime) - ConstructedWorldTime;
	TutorialCard->SetVisibility(TutorialAge < 7.0f ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	TutorialText->SetText(FText::FromString(bIsTouchLayout
		? Localized(TEXT("左摇杆推进 ｜ 右侧拖动瞄准 ｜ 弹头键开火 ｜ ↻ 换弹 ｜ ⇄ 切枪"), TEXT("LEFT STICK MOVE | SWIPE TO AIM | FIRE | RELOAD | SWAP"))
		: Localized(TEXT("WASD 移动 ｜ 鼠标索敌 ｜ 左键开火 ｜ R 换弹 ｜ Shift 切枪"), TEXT("WASD MOVE | MOUSE AIM | LMB FIRE | R RELOAD | SHIFT SWAP"))));

	const bool bWon = ShooterGS->IsMatchWon();
	VictoryOverlay->SetVisibility(bWon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (bWon)
	{
		VictoryTitle->SetText(FText::FromString(Localized(TEXT("区域肃清"), TEXT("AREA SECURED"))));
		VictoryDetails->SetText(FText::FromString(bChinese ? FString::Printf(TEXT("最终战果 %d  ｜  行动用时 %02d:%02d"), ShooterGS->GetTeamScore(), ElapsedMinutes, RemainingSeconds) : FString::Printf(TEXT("FINAL SCORE %d  |  TIME %02d:%02d"), ShooterGS->GetTeamScore(), ElapsedMinutes, RemainingSeconds)));
	}
}

void UShooterCombatHUDWidget::RefreshAmmoPips(int32 Ammo, int32 MagazineSize)
{
	const float Fraction = MagazineSize > 0 ? FMath::Clamp(static_cast<float>(Ammo) / MagazineSize, 0.0f, 1.0f) : 0.0f;
	const int32 Filled = FMath::CeilToInt(Fraction * AmmoPipWidgets.Num());
	for (int32 Index = 0; Index < AmmoPipWidgets.Num(); ++Index)
	{
		AmmoPipWidgets[Index]->SetBrushColor(Index < Filled ? Orange : FLinearColor(0.13f, 0.15f, 0.2f, 0.92f));
	}
}

FString UShooterCombatHUDWidget::Localized(const TCHAR* Chinese, const TCHAR* English) const
{
	const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>();
	return ShooterPC && ShooterPC->IsUsingChineseUI() ? FString(Chinese) : FString(English);
}
