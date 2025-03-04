// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMode\CardData.h"
#include "IngameSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UIngameSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditAnywhere, Category="Widget | Card")
	TSubclassOf<class UCardWidget> CardWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalCardBox;

	void PopulateCardWidget();
	ECardEffectType RandCardEffect();
	ECardRarity RandCardGrade();

	int32 CreateWidgetAmount = 3;

};
