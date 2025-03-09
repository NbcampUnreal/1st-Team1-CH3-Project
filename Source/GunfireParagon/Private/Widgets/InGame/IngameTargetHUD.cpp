// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameTargetHUD.h"
#include "Components\TextBlock.h"
#include "Components\ProgressBar.h"
#include "Components\SizeBox.h"

void UIngameTargetHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UIngameTargetHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngameTargetHUD::NativeDestruct()
{
	MonsterHPBar = nullptr;
	MonsterTextBlock = nullptr;
}

void UIngameTargetHUD::UpdateHP(float Current, float Max)
{
	if (MonsterHPBar)
	{
		MonsterHPBar->SetPercent(Current / Max);
	}
}

void UIngameTargetHUD::UpdateTargetName(FString TargetName)
{
	if (MonsterTextBlock)
	{
		MonsterTextBlock->SetText(FText::FromString(TargetName));
	}
}
