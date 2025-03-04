// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameCrossHairs.h"
#include "Components\Image.h"
#include "Animation\WidgetAnimation.h"

void UIngameCrossHairs::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CrossHairs.Add(CrossHairTop, FVector2D(0, -1));
	CrossHairs.Add(CrossHairRight, FVector2D(1, 0));
	CrossHairs.Add(CrossHairLeft, FVector2D(-1, 0));
	CrossHairs.Add(CrossHairBottom, FVector2D(0, 1));

	Animations.Add(AnimationTop, FVector2D(1,-1));
	Animations.Add(AnimationRight, FVector2D(1,1));
	Animations.Add(AnimationLeft, FVector2D(-1,-1));
	Animations.Add(AnimationBottom, FVector2D(-1,1));
}

void UIngameCrossHairs::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngameCrossHairs::SetCrossHairSpread(float Sperad)
{
	for (const auto& Pair : CrossHairs)
	{
		if (Pair.Key)
		{
			FVector2D Move = Pair.Value * Spread;
			Pair.Key->SetRenderTranslation(Move);
		}
	}
}

void UIngameCrossHairs::SetAnimationSpread(float Sperad)
{
	for (const auto& Pair : Animations)
	{
		if (Pair.Key)
		{
			Pair.Key->SetVisibility(ESlateVisibility::Visible);
			FVector2D Move = Pair.Value * Spread;
			Pair.Key->SetRenderTranslation(Move);
		}
	}
}

void UIngameCrossHairs::SetAnimationHitMarker(bool IsHead)
{
	SetHitMarkerImage(IsHead);
	PlayHitMarker();
}

void UIngameCrossHairs::SetHitMarkerImage(bool IsHead)
{
	FLinearColor SetColor = IsHead ? FLinearColor::Red : FLinearColor::Yellow;

	AnimationRight->SetColorAndOpacity(SetColor);
	AnimationLeft->SetColorAndOpacity(SetColor);
	AnimationBottom->SetColorAndOpacity(SetColor);
	AnimationTop->SetColorAndOpacity(SetColor);
}

void UIngameCrossHairs::PlayHitMarker()
{
	if (!HitMarkerAnim) return;

	if (IsAnimationPlaying(HitMarkerAnim))
	{
		StopAnimation(HitMarkerAnim);
	}

	PlayAnimation(HitMarkerAnim, 0.f, 1,EUMGSequencePlayMode::Forward, 1.f);
}
