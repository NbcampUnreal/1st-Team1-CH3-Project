// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/CardWidget.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Components\Button.h"

void UCardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SelectButton && !SelectButton->OnClicked.IsBound())
	{
		SelectButton->OnClicked.AddDynamic(this, &UCardWidget::SelectClicked);
	}
}

void UCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCardWidget::NativeDestruct()
{
	Super::NativeDestruct();
}


void UCardWidget::SetCardData()
{
}

void UCardWidget::SelectClicked()
{
	UE_LOG(LogTemp, Display, TEXT("CardButton Bind Test"));
	// Select Index에 대한 로직
}
