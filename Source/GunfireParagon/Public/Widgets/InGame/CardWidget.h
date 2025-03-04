// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets\InGame\SelectObjectBaseWidget.h"
#include "Widgets\UIObjectBaseData.h"
#include "GameMode\CardData.h"
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

	UFUNCTION()
	void FindCardByEnum(ECardEffectType CardType, ECardRarity CardGrade);

protected:
	UFUNCTION()
	virtual void ActivateObject() override;

	UFUNCTION()
	void UpdateCardUI(const FUIObjectBaseData& Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI DataTable")
	UDataTable* UICardInfoDataTable;

	int32 GradeValue;
};
