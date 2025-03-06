// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameSelectWidget.h"
#include "Widgets\InGame\CardWidget.h"
#include "Kismet\KismetMathLibrary.h"
#include "Kismet\GameplayStatics.h"
#include "Components\HorizontalBox.h"
#include "Components\HorizontalBoxSlot.h"

void UIngameSelectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UIngameSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PopulateCardWidget();

	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}

	}

	SetIsFocusable(true);
}

void UIngameSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UIngameSelectWidget::PopulateCardWidget()
{
	if (!CardWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIngameSelectWidget : CardWidget is Null"));
		return;
	}
	UsedEffects.Empty();

	for (int i = 0; i < CreateWidgetAmount; i++)
	{
		UCardWidget* CardWidget = CreateWidget<UCardWidget>(this, CardWidgetClass);
		CardWidget->FindCardByEnum(RandCardEffect(), RandCardGrade());

		if (UHorizontalBoxSlot* CardSlot = Cast<UHorizontalBoxSlot>(HorizontalCardBox->AddChild(CardWidget)))
		{
			CardSlot->SetPadding(FMargin(30.f, 0.f));
			CardSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			CardSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
		if (!CardWidget->SelectWidget.IsBound())
		{
			CardWidget->SelectWidget.AddDynamic(this, &UIngameSelectWidget::CloseWidget);
		}
	}
}

ECardEffectType UIngameSelectWidget::RandCardEffect()
{
	ECardEffectType EffectType;
	do
	{
		EffectType = (ECardEffectType)FMath::RandRange(0, (int32)(ECardEffectType::LastIndex)-1);
	} while (UsedEffects.Contains(EffectType));

	UsedEffects.Add(EffectType);

	return EffectType;
}

ECardRarity UIngameSelectWidget::RandCardGrade()
{
	ECardRarity RandRarity = (ECardRarity)FMath::RandRange(0, (int32)(ECardRarity::LastIndex)-1);
	return RandRarity;
}

void UIngameSelectWidget::CloseWidget()
{
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}

	}
	SetIsFocusable(false);
	RemoveFromParent();
	//RemoveFromViewport();
}
