// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterNetworkMenuWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;
class UTexture2D;

UCLASS()
class UE5_ARENACLEANUP_API UShooterNetworkMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UShooterNetworkMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnCloseClicked();

	UFUNCTION()
	void OnLanguageClicked();

	UButton* CreateMenuButton(const FString& Label);
	UTextBlock* CreateMenuText(const FString& Text, int32 Size, const FLinearColor& Color = FLinearColor::White) const;
	void RefreshLanguage();
	void CloseMenu();
	FString Localized(const TCHAR* Chinese, const TCHAR* English) const;

	UPROPERTY()
	TObjectPtr<UEditableTextBox> AddressTextBox;

	UPROPERTY()
	TObjectPtr<UTexture2D> BackgroundTexture;

	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SubtitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> HelpText;

	UPROPERTY()
	TObjectPtr<UTextBlock> HostButtonText;

	UPROPERTY()
	TObjectPtr<UTextBlock> JoinButtonText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SoloButtonText;

	UPROPERTY()
	TObjectPtr<UTextBlock> LanguageButtonText;

	bool bUseChinese = true;
};
