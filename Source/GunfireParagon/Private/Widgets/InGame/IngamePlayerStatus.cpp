// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngamePlayerStatus.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Widgets/DataAssets/InGameStateDataAsset.h"

void UIngamePlayerStatus::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!CurrentTextures)
	{
		UE_LOG(LogTemp, Warning, TEXT("Texture DataAsset Is Missing"));
	}
}

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

	if (HPTextBlock)
	{
		FString HPString = FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP);
		HPTextBlock->SetText(FText::FromString(HPString));
	}

	if (ShieldBar && MaxShield > 0)
	{
		float ShieldPercent = (float)CurrentShield / (float)MaxShield;
		ShieldBar->SetPercent(ShieldPercent);
	}

	if (ShieldTextBlock)
	{
		FString ShieldString = FString::Printf(TEXT("%d / %d"), CurrentShield, MaxShield);
		ShieldTextBlock->SetText(FText::FromString(ShieldString));
	}
}

void UIngamePlayerStatus::SetCurrentHP(const int32& UpdateHP, const int32& UpdateMaxHP)
{
	MaxHP = UpdateMaxHP;
	CurrentHP = FMath::Clamp(UpdateHP, 0, MaxHP);
	UpdatePlayerState();
}

void UIngamePlayerStatus::SetCurrentShield(const int32& UpdateShield, const int32& UpdateMaxShield)
{
	MaxShield = UpdateMaxShield;
	CurrentShield = FMath::Clamp(UpdateShield, 0, MaxShield);
	UpdatePlayerState();
}

void UIngamePlayerStatus::SetCurrentPlayerTexture()
{
	// 플레이어 Base 기반의 다른 플레이어가 있을 경우. 없다면 WBP 단계에서 Texture 적용
}
