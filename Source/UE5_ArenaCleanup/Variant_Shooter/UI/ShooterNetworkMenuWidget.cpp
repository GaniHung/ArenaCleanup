// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterNetworkMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "ShooterPlayerController.h"

UShooterNetworkMenuWidget::UShooterNetworkMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	BackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/ArenaCleanup/UI/T_ArenaMenuBackground.T_ArenaMenuBackground"));
}

void UShooterNetworkMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("StartMenuRoot"));
	WidgetTree->RootWidget = Root;

	UImage* Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("ArenaBackground"));
	if (BackgroundTexture)
	{
		Background->SetBrushFromTexture(BackgroundTexture, true);
	}
	Background->SetColorAndOpacity(FLinearColor::White);
	if (UCanvasPanelSlot* BackgroundSlot = Root->AddChildToCanvas(Background))
	{
		BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		BackgroundSlot->SetOffsets(FMargin(0.0f));
	}

	UBorder* Shade = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CinematicShade"));
	Shade->SetBrushColor(FLinearColor(0.005f, 0.012f, 0.025f, 0.38f));
	if (UCanvasPanelSlot* ShadeSlot = Root->AddChildToCanvas(Shade))
	{
		ShadeSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		ShadeSlot->SetOffsets(FMargin(0.0f));
	}

	UBorder* MenuCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StartMenuCard"));
	MenuCard->SetBrushColor(FLinearColor(0.008f, 0.018f, 0.04f, 0.94f));
	MenuCard->SetPadding(FMargin(48.0f, 42.0f));
	if (UCanvasPanelSlot* CardSlot = Root->AddChildToCanvas(MenuCard))
	{
		CardSlot->SetAnchors(FAnchors(0.06f, 0.5f));
		CardSlot->SetAlignment(FVector2D(0.0f, 0.5f));
		CardSlot->SetSize(FVector2D(610.0f, 760.0f));
	}

	UVerticalBox* Panel = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StartMenuContent"));
	MenuCard->SetContent(Panel);

	TitleText = CreateMenuText(TEXT("竞技场：肃清行动"), 52, FLinearColor(0.92f, 0.98f, 1.0f, 1.0f));
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(TitleText))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
	}

	SubtitleText = CreateMenuText(TEXT("ARENA CLEANUP"), 22, FLinearColor(0.0f, 0.86f, 1.0f, 1.0f));
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(SubtitleText))
	{
		ContentSlot->SetPadding(FMargin(2.0f, 0.0f, 0.0f, 34.0f));
	}

	HelpText = CreateMenuText(TEXT("三轮攻势即将压境。锁定目标，守住阵地，和队友活着清场。"), 20, FLinearColor(0.72f, 0.82f, 0.94f, 1.0f));
	HelpText->SetAutoWrapText(true);
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(HelpText))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 30.0f));
	}

	UButton* HostButton = CreateMenuButton(TEXT("建立作战房间"));
	HostButtonText = Cast<UTextBlock>(HostButton->GetContent());
	HostButton->OnClicked.AddDynamic(this, &UShooterNetworkMenuWidget::OnHostClicked);
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(HostButton))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
		ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	AddressTextBox = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("AddressTextBox"));
	AddressTextBox->SetText(FText::FromString(TEXT("127.0.0.1:7777")));
	AddressTextBox->SetHintText(FText::FromString(TEXT("输入主机地址，例如 10.156.246.35:7777")));
	AddressTextBox->SetForegroundColor(FLinearColor::White);
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(AddressTextBox))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
		ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	UButton* JoinButton = CreateMenuButton(TEXT("接入战局"));
	JoinButtonText = Cast<UTextBlock>(JoinButton->GetContent());
	JoinButton->OnClicked.AddDynamic(this, &UShooterNetworkMenuWidget::OnJoinClicked);
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(JoinButton))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
		ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	UButton* SoloButton = CreateMenuButton(TEXT("单兵出击"));
	SoloButtonText = Cast<UTextBlock>(SoloButton->GetContent());
	SoloButton->OnClicked.AddDynamic(this, &UShooterNetworkMenuWidget::OnCloseClicked);
	if (UVerticalBoxSlot* ContentSlot = Panel->AddChildToVerticalBox(SoloButton))
	{
		ContentSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
		ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	UButton* LanguageButton = CreateMenuButton(TEXT("中文 / ENGLISH"));
	LanguageButtonText = Cast<UTextBlock>(LanguageButton->GetContent());
	LanguageButton->SetBackgroundColor(FLinearColor(0.12f, 0.16f, 0.22f, 0.92f));
	LanguageButton->OnClicked.AddDynamic(this, &UShooterNetworkMenuWidget::OnLanguageClicked);
	if (UCanvasPanelSlot* LanguageSlot = Root->AddChildToCanvas(LanguageButton))
	{
		LanguageSlot->SetAnchors(FAnchors(1.0f, 0.0f));
		LanguageSlot->SetAlignment(FVector2D(1.0f, 0.0f));
		LanguageSlot->SetPosition(FVector2D(-44.0f, 36.0f));
		LanguageSlot->SetSize(FVector2D(250.0f, 58.0f));
	}

	RefreshLanguage();
}

void UShooterNetworkMenuWidget::OnHostClicked()
{
	if (AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		ShooterPC->HostLanSession();
	}
}

void UShooterNetworkMenuWidget::OnJoinClicked()
{
	if (AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		const FString Address = AddressTextBox ? AddressTextBox->GetText().ToString() : FString(TEXT("127.0.0.1:7777"));
		ShooterPC->JoinLanSession(Address);
	}
}

void UShooterNetworkMenuWidget::OnCloseClicked()
{
	if (AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		ShooterPC->PlaySoloFromMenu();
	}
}

void UShooterNetworkMenuWidget::OnLanguageClicked()
{
	bUseChinese = !bUseChinese;
	RefreshLanguage();
}

UButton* UShooterNetworkMenuWidget::CreateMenuButton(const FString& Label)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor(0.0f, 0.43f, 0.65f, 0.96f));
	UTextBlock* ButtonText = CreateMenuText(Label, 26);
	ButtonText->SetJustification(ETextJustify::Center);
	Button->AddChild(ButtonText);
	return Button;
}

UTextBlock* UShooterNetworkMenuWidget::CreateMenuText(const FString& Text, int32 Size, const FLinearColor& Color) const
{
	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TextBlock->SetText(FText::FromString(Text));
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	TextBlock->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", Size));
	TextBlock->SetShadowOffset(FVector2D(1.5f, 1.5f));
	TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
	return TextBlock;
}

void UShooterNetworkMenuWidget::RefreshLanguage()
{
	if (AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		ShooterPC->SetUseChineseUI(bUseChinese);
	}
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(Localized(TEXT("竞技场：肃清行动"), TEXT("ARENA CLEANUP"))));
	}
	if (SubtitleText)
	{
		SubtitleText->SetText(FText::FromString(Localized(TEXT("双人协同 · 三波歼灭"), TEXT("CO-OP EXTERMINATION PROTOCOL"))));
	}
	if (HelpText)
	{
		HelpText->SetText(FText::FromString(Localized(TEXT("三轮攻势即将压境。锁定目标，守住阵地，和队友活着清场。"), TEXT("Three assault waves incoming. Lock targets, hold the arena, and make it out together."))));
	}
	if (HostButtonText)
	{
		HostButtonText->SetText(FText::FromString(Localized(TEXT("建立作战房间"), TEXT("HOST CO-OP MISSION"))));
	}
	if (JoinButtonText)
	{
		JoinButtonText->SetText(FText::FromString(Localized(TEXT("接入战局"), TEXT("JOIN BY IP"))));
	}
	if (SoloButtonText)
	{
		SoloButtonText->SetText(FText::FromString(Localized(TEXT("单兵出击"), TEXT("DEPLOY SOLO"))));
	}
	if (LanguageButtonText)
	{
		LanguageButtonText->SetText(FText::FromString(bUseChinese ? TEXT("中文 / ENGLISH") : TEXT("ENGLISH / 中文")));
	}
	if (AddressTextBox)
	{
		AddressTextBox->SetHintText(FText::FromString(Localized(TEXT("输入主机地址，例如 10.156.246.35:7777"), TEXT("HOST ADDRESS, E.G. 10.156.246.35:7777"))));
	}
}

void UShooterNetworkMenuWidget::CloseMenu()
{
	RemoveFromParent();
}

FString UShooterNetworkMenuWidget::Localized(const TCHAR* Chinese, const TCHAR* English) const
{
	return bUseChinese ? FString(Chinese) : FString(English);
}
