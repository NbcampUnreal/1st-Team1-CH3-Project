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
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	
	UFUNCTION()
	void UpdateHP(float Current, float Max);

	UFUNCTION()
	void UpdateTargetName(FString TargetName);

private:
	UPROPERTY(meta =(BindWidget))
	UProgressBar* MonsterHPBar;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* MonsterTextBlock;
};
