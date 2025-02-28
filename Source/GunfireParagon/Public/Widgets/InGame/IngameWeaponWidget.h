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

	UPROPERTY(EditAnywhere, Category = "Textures")
	class UInGameStateDataAsset* CurrentTextures;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float DodgeCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float SkillCoolTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UITest")
	int32 MaxAmmo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UITest")
	int32 CurrentGrenade;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DashCoolDownAnim;

	void UpdateAmmoState();
	void PlayDashCoolDownAnim(float DashCoolDown);
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentAmmo(const int32& UpdateAmmo, const int32& UpdateMaxAmmo);	

	UFUNCTION(BlueprintCallable)
	void SetDashCoolDown(float DashCoolDown);
};
