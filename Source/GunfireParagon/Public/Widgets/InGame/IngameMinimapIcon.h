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
	virtual void NativeDestruct() override;
public:
	UFUNCTION()
	void SetWorldLocation(FVector2D RenderPosition);

	UFUNCTION()
	void SetIconTexture(UTexture2D* Texture);
	
	UFUNCTION()
	void SetVisibilityBasedOnDistance(float Distance, float MaxDistance);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
};
