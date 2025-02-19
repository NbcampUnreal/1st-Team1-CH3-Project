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

UCLASS()
class GUNFIREPARAGON_API UIngameWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float DodgeCoolTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float SkillCoolTime;

public:
	//Test Case
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateWeaponState();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 CurrentGrenade;
};
