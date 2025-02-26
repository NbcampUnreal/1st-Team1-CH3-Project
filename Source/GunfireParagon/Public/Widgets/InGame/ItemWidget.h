// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets\InGame\SelectObjectBaseWidget.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UItemWidget : public USelectObjectBaseWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnOverlapButton();
protected:
	virtual void ActivateObject() override;
	
	//UPROPERTY(meta=(BindWidget))
	//UImage* SoldOutImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;



};
