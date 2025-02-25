// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UButton;

UCLASS()
class GUNFIREPARAGON_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* CardTexture;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CardName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CardDescription;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;
	
public:
	// DataAsset/DataTable을 사용한 데이터 참조
	// Card(Passive) FString Contains를 사용한 비교, 존재시 해당 데이터테이블 값 참조 
	void SetCardData();

private:
	UFUNCTION()
	void SelectClicked();
};
