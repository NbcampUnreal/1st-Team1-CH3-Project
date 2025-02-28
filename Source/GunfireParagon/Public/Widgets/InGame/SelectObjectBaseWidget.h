// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectObjectBaseWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UButton;
class UImage;

UCLASS()
class GUNFIREPARAGON_API USelectObjectBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnClickedSelectButton();
protected:
	UPROPERTY(meta =(BindWidget))
	UButton* SelectButton;
	
	UPROPERTY(meta = (BindWidget))
	UImage* ObjectImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionText;

protected:
	virtual void ActivateObject() PURE_VIRTUAL(USelectObjectBaseWidget::ActivateObject, );
};
