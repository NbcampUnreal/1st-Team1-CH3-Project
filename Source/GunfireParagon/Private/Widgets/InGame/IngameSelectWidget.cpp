// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameSelectWidget.h"
#include "Widgets\InGame\CardWidget.h"
#include "Kismet\KismetMathLibrary.h"
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
	}
}

ECardEffectType UIngameSelectWidget::RandCardEffect()
{
	ECardEffectType EffectType = (ECardEffectType)FMath::RandRange(0, (int32)(ECardEffectType::LastIndex)-1);
	UE_LOG(LogTemp, Warning, TEXT("Effect Type : %d"),(int32)EffectType);
	return EffectType;

	//int32 MinValue = static_cast<int32>(0); // 첫 번째 값
	//int32 MaxValue = static_cast<int32>(ECardEffectType::LastIndex); // 마지막 값

	//int32 RandomIndex = FMath::RandRange(MinValue, MaxValue-1);
	//ECardEffectType EffectType = static_cast<ECardEffectType>(RandomIndex);

	//UE_LOG(LogTemp, Warning, TEXT("Effect Type Generated: %d"), (int32)EffectType);
	//return EffectType;
}

ECardRarity UIngameSelectWidget::RandCardGrade()
{
	ECardRarity RandRarity = (ECardRarity)FMath::RandRange(0, (int32)(ECardRarity::LastIndex)-1);
	UE_LOG(LogTemp, Warning, TEXT("Effect Type : %d"), (int32)RandRarity);
	return RandRarity;

	//int32 MinValue = static_cast<int32>(0); // 첫 번째 값
	//int32 MaxValue = static_cast<int32>(ECardEffectType::LastIndex); // 마지막 값

	//int32 RandomIndex = FMath::RandRange(MinValue, MaxValue - 1);
	//ECardRarity RandRarity = static_cast<ECardRarity>(RandomIndex);

	//UE_LOG(LogTemp, Warning, TEXT("Effect Type Generated: %d"), (int32)RandRarity);
	//return RandRarity;
}
