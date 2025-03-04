// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Ingame/IngameWeaponWidget.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Components\ProgressBar.h"
#include "Animation\WidgetAnimation.h"
#include "Widgets\DataAssets\WeaponDataAsset.h"
#include "Actor\Weapon\CGunBase.h"

void UIngameWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WeaponDataAssets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Texture is Missing"));
	}
}

void UIngameWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngameWeaponWidget::UpdateAmmoState()
{
	if (AmmoTextBlock)
	{
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

void UIngameWeaponWidget::UpdateWeaponTexture(ACGunBase* CurrentWeapon)
{
	TSubclassOf<ACGunBase> CurrentWeaponClass = CurrentWeapon->GetClass();

	for (const auto& WeaponData : WeaponDataAssets->WeaponTextures)
	{
		if (WeaponData.WeaponClass == CurrentWeaponClass)
		{
			if (WeaponTexture && WeaponData.WeaponTexture)
			{
				WeaponTexture->SetBrushFromTexture(WeaponData.WeaponTexture);
			}
			return;
		}
	}

	if (WeaponTexture)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Texture Not Binding DataAsstes"));
		WeaponTexture->SetBrushFromTexture(nullptr);
	}
}

void UIngameWeaponWidget::PlayDashCoolDownAnim(float DashCoolDown)
{
	StopAnimation(DashCoolDownAnim);
	
	DashCoolDownBar->SetPercent(1.0f);

	float AnimEndTime = DashCoolDownAnim->GetEndTime();

	float PlayRate = AnimEndTime / DashCoolDown;

	PlayAnimation(DashCoolDownAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
}


void UIngameWeaponWidget::SetCurrentAmmo(const int32& UpdateAmmo, const int32& UpdateMaxAmmo)
{
	CurrentAmmo = UpdateAmmo;
	MaxAmmo = UpdateMaxAmmo;
	UpdateAmmoState();
}

void UIngameWeaponWidget::SetDashCoolDown(float DashCoolDown)
{
	if (!DashCoolDownBar || !DashCoolDownAnim)
	{
		UE_LOG(LogTemp, Display, TEXT("DashCoolDownBar||Anim is Null"));
		return;
	}

	PlayDashCoolDownAnim(DashCoolDown);
}

void UIngameWeaponWidget::SetCurrentWeaponTexture(ACGunBase* CurrentWeapon)
{
	if (!WeaponDataAssets)
	{
		UE_LOG(LogTemp, Warning, TEXT("RunTime WeaponDataAssets Not Binding."));
		return;
	}

	UpdateWeaponTexture(CurrentWeapon);
}
