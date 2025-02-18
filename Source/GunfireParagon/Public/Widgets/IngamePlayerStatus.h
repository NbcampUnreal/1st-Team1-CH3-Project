// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngamePlayerStatus.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class GUNFIREPARAGON_API UIngamePlayerStatus : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldTextBlock;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* CurrentPlayerTexture;

public:
	// Todo : 로직 테스트용이며, 실제로는 Player의 CurrentHP, MaxHP, CurrentShield, MaxShield를 사용하여 업데이트 할 것입니다.
	// Test

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 CurrentHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 CurrentShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UITest")
	int32 MaxShield;
};
