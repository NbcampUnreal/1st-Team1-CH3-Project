// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/DashEffectWidget.h"
#include "Components\Image.h"

void UDashEffectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDashEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (DashImage)
	{
		DashImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UDashEffectWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDashEffectWidget::ConvertVisibility(bool IsDash)
{
	IsDash ? DashImage->SetVisibility(ESlateVisibility::Visible) : DashImage->SetVisibility(ESlateVisibility::Hidden);
}
