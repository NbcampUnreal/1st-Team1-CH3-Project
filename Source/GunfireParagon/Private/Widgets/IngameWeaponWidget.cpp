// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/IngameWeaponWidget.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Widgets/DataAssets/InGameStateDataAsset.h"

void UIngameWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!CurrentTextures)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Texture is Missing"));
	}
}

void UIngameWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateWeaponState();
}

void UIngameWeaponWidget::UpdateWeaponState()
{
	if (AmmoTextBlock)
	{
		//Setter
		CurrentAmmo = FMath::Clamp(CurrentAmmo, 0, MaxAmmo);

		FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
		AmmoTextBlock->SetText(FText::FromString(AmmoString));
	}

	if (GrenadeTextBlock)
	{
		FString GrenadeString = FString::Printf(TEXT("%d"), CurrentGrenade);
		GrenadeTextBlock->SetText(FText::FromString(GrenadeString));
	}
}
