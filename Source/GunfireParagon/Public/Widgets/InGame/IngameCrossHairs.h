// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameCrossHairs.generated.h"

/**
 * 
 */

class UImage;

UCLASS()
class GUNFIREPARAGON_API UIngameCrossHairs : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	//TestCase
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCrossHairSpread(float Sperad);

	UFUNCTION(BlueprintCallable)
	void SetAnimationSpread(float Sperad);

	UFUNCTION()
	void SetAnimationVisible();
protected:
	// TODO:
	// 메모리 적으로는 x2의 방식을 사용하게된다. 비효율적
	// 성능테스트 후 문제 발생 시 최적화 우선순위

	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairTop;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairRight;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* CrossHairBottom;

	UPROPERTY(meta = (BindWidget))
	UImage* AnimationTop;
	UPROPERTY(meta = (BindWidget))
	UImage* AnimationRight;
	UPROPERTY(meta = (BindWidget))
	UImage* AnimationLeft;
	UPROPERTY(meta = (BindWidget))
	UImage* AnimationBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spread")
	float Spread;

private:
	TMap<UImage*,FVector2D> CrossHairs;
	TMap<UImage*,FVector2D> Animations;
};
