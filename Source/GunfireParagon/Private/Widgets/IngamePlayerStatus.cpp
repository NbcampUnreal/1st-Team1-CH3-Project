// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/IngamePlayerStatus.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UIngamePlayerStatus::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePlayerState();
}

void UIngamePlayerStatus::UpdatePlayerState()
{
	if (HPBar && MaxHP > 0)
	{
		float HPPercent = (float)CurrentHP / (float)MaxHP;
		HPBar->SetPercent(HPPercent);
	}

	if (ShieldBar && MaxShield > 0)
	{
		float ShieldPercent = (float)CurrentShield / (float)MaxShield;
		ShieldBar->SetPercent(ShieldPercent);
	}

	if (HPTextBlock)
	{
		FString HPString = FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP);
		HPTextBlock->SetText(FText::FromString(HPString));
	}

	if (ShieldTextBlock)
	{
		FString ShieldString = FString::Printf(TEXT("%d / %d"), CurrentShield, MaxShield);
		ShieldTextBlock->SetText(FText::FromString(ShieldString));
	}
}

void UIngamePlayerStatus::SetCurrentHP(int32 UpdateHP, int32 UpdateMaxHP)
{
	MaxHP = UpdateMaxHP;
	CurrentHP = FMath::Clamp(UpdateHP, 0, MaxHP);
	UpdatePlayerState();
}

void UIngamePlayerStatus::SetCurrentShield(int32 UpdateShield, int32 UpdateMaxShield)
{
	MaxShield = UpdateMaxShield;
	CurrentShield = FMath::Clamp(UpdateShield, 0, MaxShield);
	UpdatePlayerState();
}
