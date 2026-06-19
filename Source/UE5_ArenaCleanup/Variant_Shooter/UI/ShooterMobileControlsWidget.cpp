// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterMobileControlsWidget.h"

#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Engine/Texture2D.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

namespace
{
void DrawTouchBox(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color)
{
	FSlateDrawElement::MakeBox(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Position, Size),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		Color);
}

void DrawTouchCircle(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Center, float Radius, const FLinearColor& Color, UTexture2D* CircleTexture)
{
	FSlateBrush CircleBrush;
	CircleBrush.SetResourceObject(CircleTexture);
	CircleBrush.ImageSize = FVector2D(Radius * 2.0f);
	CircleBrush.DrawAs = ESlateBrushDrawType::Image;
	FSlateDrawElement::MakeBox(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Center - FVector2D(Radius), FVector2D(Radius * 2.0f)),
		CircleTexture ? &CircleBrush : FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		Color);
}

void DrawBulletSymbol(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Center)
{
	DrawTouchBox(Geometry, DrawElements, Layer, Center + FVector2D(-9.0f, -7.0f), FVector2D(18.0f, 29.0f), FLinearColor::White);
	DrawTouchBox(Geometry, DrawElements, Layer, Center + FVector2D(-13.0f, 22.0f), FVector2D(26.0f, 6.0f), FLinearColor::White);
	TArray<FVector2D> TipPoints;
	TipPoints.Add(Center + FVector2D(-9.0f, -7.0f));
	TipPoints.Add(Center + FVector2D(0.0f, -23.0f));
	TipPoints.Add(Center + FVector2D(9.0f, -7.0f));
	FSlateDrawElement::MakeLines(DrawElements, Layer, Geometry.ToPaintGeometry(), TipPoints, ESlateDrawEffect::None, FLinearColor::White, true, 5.0f);
}

void DrawTouchText(const FGeometry& Geometry, FSlateWindowElementList& DrawElements, int32 Layer, const FVector2D& Position, const FString& Text, int32 Size = 18)
{
	FSlateDrawElement::MakeText(
		DrawElements,
		Layer,
		Geometry.ToPaintGeometry(Position, FVector2D(220.0f, Size + 10.0f)),
		Text,
		FCoreStyle::GetDefaultFontStyle("Bold", Size),
		ESlateDrawEffect::None,
		FLinearColor::White);
}
}

UShooterMobileControlsWidget::UShooterMobileControlsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	TouchCircleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/ArenaCleanup/UI/T_TouchCircle.T_TouchCircle"));
}

void UShooterMobileControlsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MovePointerIndex != INDEX_NONE)
	{
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoMove(MoveVector.X, -MoveVector.Y);
		}
	}
}

FReply UShooterMobileControlsWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		if (ShooterPC->IsNetworkMenuVisible())
		{
			return FReply::Unhandled();
		}
	}

	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());
	const FVector2D Size = InGeometry.GetLocalSize();
	const int32 PointerIndex = InGestureEvent.GetPointerIndex();

	if (IsInsideReloadButton(LocalPosition, Size))
	{
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoReload();
		}
		return FReply::Handled();
	}

	if (IsInsideSwitchButton(LocalPosition, Size))
	{
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoSwitchWeapon();
		}
		return FReply::Handled();
	}

	if (LocalPosition.X < Size.X * 0.46f && LocalPosition.Y > Size.Y * 0.38f && MovePointerIndex == INDEX_NONE)
	{
		MovePointerIndex = PointerIndex;
		MoveCenter = LocalPosition;
		UpdateMoveStick(LocalPosition);
		return FReply::Handled();
	}

	if (IsInsideRightFireZone(LocalPosition, Size) && FirePointerIndex == INDEX_NONE)
	{
		FirePointerIndex = PointerIndex;
		FireCenter = LocalPosition;
		FireVector = FVector2D::ZeroVector;
		LastLookPosition = LocalPosition;
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoStartFiring();
		}
		return FReply::Handled();
	}

	if (LocalPosition.X > Size.X * 0.38f && LookPointerIndex == INDEX_NONE)
	{
		LookPointerIndex = PointerIndex;
		LastLookPosition = LocalPosition;
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UShooterMobileControlsWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		if (ShooterPC->IsNetworkMenuVisible())
		{
			return FReply::Unhandled();
		}
	}

	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InGestureEvent.GetScreenSpacePosition());
	const int32 PointerIndex = InGestureEvent.GetPointerIndex();

	if (PointerIndex == MovePointerIndex)
	{
		UpdateMoveStick(LocalPosition);
		return FReply::Handled();
	}

	if (PointerIndex == FirePointerIndex)
	{
		const FVector2D Delta = LocalPosition - LastLookPosition;
		FireVector = (LocalPosition - FireCenter).GetClampedToMaxSize(StickRadius);
		LastLookPosition = LocalPosition;
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoAim(Delta.X * LookSensitivity, Delta.Y * LookSensitivity);
		}
		return FReply::Handled();
	}

	if (PointerIndex == LookPointerIndex)
	{
		UpdateLookDelta(LocalPosition);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UShooterMobileControlsWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if (const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		if (ShooterPC->IsNetworkMenuVisible())
		{
			return FReply::Unhandled();
		}
	}

	ReleaseTouch(InGestureEvent.GetPointerIndex());
	return FReply::Handled();
}

int32 UShooterMobileControlsWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	const FVector2D Size = AllottedGeometry.GetLocalSize();

	if (const AShooterPlayerController* ShooterPC = GetOwningPlayer<AShooterPlayerController>())
	{
		if (ShooterPC->IsNetworkMenuVisible())
		{
			return BaseLayer;
		}
	}

	const FVector2D DefaultMoveCenter(180.0f, Size.Y - 180.0f);
	const FVector2D DrawMoveCenter = MovePointerIndex == INDEX_NONE ? DefaultMoveCenter : MoveCenter;
	const FVector2D MoveKnob = DrawMoveCenter + MoveVector * StickRadius;
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 1, DrawMoveCenter, StickRadius, FLinearColor(0.12f, 0.58f, 1.0f, 0.14f), TouchCircleTexture);
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 2, MoveKnob, 46.0f, FLinearColor(0.28f, 0.82f, 1.0f, 0.42f), TouchCircleTexture);

	const FVector2D DefaultFireCenter(Size.X - 190.0f, Size.Y - 190.0f);
	const FVector2D DrawFireCenter = FirePointerIndex == INDEX_NONE ? DefaultFireCenter : FireCenter;
	const FVector2D FireKnob = DrawFireCenter + FireVector.GetClampedToMaxSize(StickRadius);
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 1, DrawFireCenter, StickRadius, FLinearColor(1.0f, 0.22f, 0.08f, 0.14f), TouchCircleTexture);
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 2, FireKnob, 54.0f, FLinearColor(1.0f, 0.34f, 0.12f, 0.44f), TouchCircleTexture);
	DrawBulletSymbol(AllottedGeometry, OutDrawElements, BaseLayer + 3, FireKnob);

	const FVector2D ReloadCenter(Size.X - 300.0f, Size.Y - 350.0f);
	const FVector2D SwitchCenter(Size.X - 116.0f, Size.Y - 350.0f);
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 2, ReloadCenter, 66.0f, FLinearColor(0.04f, 0.07f, 0.12f, 0.38f), TouchCircleTexture);
	DrawTouchText(AllottedGeometry, OutDrawElements, BaseLayer + 3, ReloadCenter + FVector2D(-23.0f, -30.0f), TEXT("↻"), 42);
	DrawTouchCircle(AllottedGeometry, OutDrawElements, BaseLayer + 2, SwitchCenter, 66.0f, FLinearColor(0.04f, 0.07f, 0.12f, 0.38f), TouchCircleTexture);
	DrawTouchText(AllottedGeometry, OutDrawElements, BaseLayer + 3, SwitchCenter + FVector2D(-27.0f, -30.0f), TEXT("⇄"), 42);

	return BaseLayer + 4;
}

AShooterCharacter* UShooterMobileControlsWidget::GetShooterCharacter() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	return PlayerController ? Cast<AShooterCharacter>(PlayerController->GetPawn()) : nullptr;
}

void UShooterMobileControlsWidget::UpdateMoveStick(const FVector2D& LocalPosition)
{
	MoveVector = (LocalPosition - MoveCenter).GetClampedToMaxSize(StickRadius) / StickRadius;
}

void UShooterMobileControlsWidget::UpdateLookDelta(const FVector2D& LocalPosition)
{
	const FVector2D Delta = LocalPosition - LastLookPosition;
	LastLookPosition = LocalPosition;
	if (AShooterCharacter* Character = GetShooterCharacter())
	{
		Character->DoAim(Delta.X * LookSensitivity, Delta.Y * LookSensitivity);
	}
}

bool UShooterMobileControlsWidget::IsInsideReloadButton(const FVector2D& LocalPosition, const FVector2D& Size) const
{
	return FVector2D::Distance(LocalPosition, FVector2D(Size.X - 300.0f, Size.Y - 350.0f)) <= 72.0f;
}

bool UShooterMobileControlsWidget::IsInsideSwitchButton(const FVector2D& LocalPosition, const FVector2D& Size) const
{
	return FVector2D::Distance(LocalPosition, FVector2D(Size.X - 116.0f, Size.Y - 350.0f)) <= 72.0f;
}

bool UShooterMobileControlsWidget::IsInsideRightFireZone(const FVector2D& LocalPosition, const FVector2D& Size) const
{
	return LocalPosition.X > Size.X * 0.54f && LocalPosition.Y > Size.Y * 0.42f;
}

void UShooterMobileControlsWidget::ReleaseTouch(int32 PointerIndex)
{
	if (PointerIndex == MovePointerIndex)
	{
		MovePointerIndex = INDEX_NONE;
		MoveVector = FVector2D::ZeroVector;
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoMove(0.0f, 0.0f);
		}
	}

	if (PointerIndex == FirePointerIndex)
	{
		FirePointerIndex = INDEX_NONE;
		FireVector = FVector2D::ZeroVector;
		if (AShooterCharacter* Character = GetShooterCharacter())
		{
			Character->DoStopFiring();
		}
	}

	if (PointerIndex == LookPointerIndex)
	{
		LookPointerIndex = INDEX_NONE;
	}
}
