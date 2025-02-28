// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashEffectWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UDashEffectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void ConvertVisibility(bool IsDash);
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* DashImage;
};
