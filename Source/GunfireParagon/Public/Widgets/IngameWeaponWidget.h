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
};
