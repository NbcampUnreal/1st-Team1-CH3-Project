// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/SelectObjectBaseWidget.h"
#include "Components\Button.h"
#include "Components\TextBlock.h"
#include "Components\Image.h"

void USelectObjectBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SelectButton && !SelectButton->OnClicked.IsBound())
	{
		SelectButton->OnClicked.AddDynamic(this, &USelectObjectBaseWidget::OnClickedSelectButton);
	}
}

void USelectObjectBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USelectObjectBaseWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void USelectObjectBaseWidget::OnClickedSelectButton()
{
	ActivateObject();
}