// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameWeaponWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
class UProgressBar;

UCLASS()
class GUNFIREPARAGON_API UIngameWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* GrenadeTexture;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponTexture;

	UPROPERTY(meta =(BindWidget))
	UImage* SkillTexture;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* DashCoolDownBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UWeaponDataAsset* WeaponDataAssets;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UITest")
	int32 MaxAmmo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UITest")
	int32 CurrentGrenade;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DashCoolDownAnim;

	void UpdateAmmoState();
	void UpdateWeaponTexture(class ACGunBase* CurrentWeapon);
	void PlayDashCoolDownAnim(float DashCoolDown);
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentAmmo(const int32& UpdateAmmo, const int32& UpdateMaxAmmo);	

	UFUNCTION(BlueprintCallable)
	void SetDashCoolDown(float DashCoolDown);

	UFUNCTION(BlueprintCallable)
	void SetCurrentWeaponTexture(class ACGunBase* CurrentWeapon);
};
