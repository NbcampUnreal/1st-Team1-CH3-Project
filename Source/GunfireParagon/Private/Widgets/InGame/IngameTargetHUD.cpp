// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameTargetHUD.h"
#include "Components\TextBlock.h"
#include "Components\ProgressBar.h"
#include "Components\SizeBox.h"

void UIngameTargetHUD::UpdateHP(float Current, float Max)
{
	if (MonsterHPBar)
	{
		MonsterHPBar->SetPercent(Current / Max);
	}
}

void UIngameTargetHUD::UpdateScale(float Distance)
{
}
