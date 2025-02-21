// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameCrossHairs.h"
#include "Components\Image.h"

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

	Spread = 50.f;
	SetAnimationVisible();
}

void UIngameCrossHairs::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngameCrossHairs::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

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

void UIngameCrossHairs::SetAnimationVisible()
{
	for (const auto& Pair : Animations)
	{
		if (Pair.Key)
		{
			Pair.Key->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
