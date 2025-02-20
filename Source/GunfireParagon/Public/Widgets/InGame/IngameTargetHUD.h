// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameTargetHUD.generated.h"

/**
 * 
 */

 class UProgressBar;
 class UTextBlock;
 class USizeBox;

UCLASS()
class GUNFIREPARAGON_API UIngameTargetHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateHP(float Current, float Max);
	UFUNCTION()
	void UpdateScale(float Distance);

private:
	UPROPERTY(meta =(BindWidget))
	USizeBox* HUDSizeBox;

	UPROPERTY(meta =(BindWidget))
	UProgressBar* MonsterHPBar;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* MonsterTextBlock;
};
