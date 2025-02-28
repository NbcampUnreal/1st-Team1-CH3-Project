// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/ItemWidget.h"
#include "Components\Button.h"

void UItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SelectButton && !SelectButton->OnHovered.IsBound())
	{
		SelectButton->OnHovered.AddDynamic(this, &UItemWidget::OnOverlapButton);
	}
}

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemWidget::NativeDestruct()
{
	Super::NativeConstruct();
}

void UItemWidget::OnOverlapButton()
{
	// Show Description UI
}

void UItemWidget::ActivateObject()
{
}
