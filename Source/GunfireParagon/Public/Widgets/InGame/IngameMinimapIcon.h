// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameMinimapIcon.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UIngameMinimapIcon : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetWorldLocation(const FVector& NewLocation);



private:
	FVector WorldLocation;
	void UpdateIconPosition();
};
