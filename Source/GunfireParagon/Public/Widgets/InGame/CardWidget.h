// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets\InGame\SelectObjectBaseWidget.h"
#include "CardWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UButton;

UCLASS()
class GUNFIREPARAGON_API UCardWidget : public USelectObjectBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	virtual void ActivateObject() override;
};
