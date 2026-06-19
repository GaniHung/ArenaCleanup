// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterMobileControlsWidget.generated.h"

class AShooterCharacter;
class UTexture2D;

UCLASS()
class UE5_ARENACLEANUP_API UShooterMobileControlsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UShooterMobileControlsWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	AShooterCharacter* GetShooterCharacter() const;
	void UpdateMoveStick(const FVector2D& LocalPosition);
	void UpdateLookDelta(const FVector2D& LocalPosition);
	bool IsInsideReloadButton(const FVector2D& LocalPosition, const FVector2D& Size) const;
	bool IsInsideSwitchButton(const FVector2D& LocalPosition, const FVector2D& Size) const;
	bool IsInsideRightFireZone(const FVector2D& LocalPosition, const FVector2D& Size) const;
	void ReleaseTouch(int32 PointerIndex);

	int32 MovePointerIndex = INDEX_NONE;
	int32 FirePointerIndex = INDEX_NONE;
	int32 LookPointerIndex = INDEX_NONE;

	FVector2D MoveCenter = FVector2D::ZeroVector;
	FVector2D MoveVector = FVector2D::ZeroVector;
	FVector2D FireCenter = FVector2D::ZeroVector;
	FVector2D FireVector = FVector2D::ZeroVector;
	FVector2D LastLookPosition = FVector2D::ZeroVector;

	float StickRadius = 122.0f;
	float LookSensitivity = 0.08f;

	UPROPERTY()
	TObjectPtr<UTexture2D> TouchCircleTexture;
};
