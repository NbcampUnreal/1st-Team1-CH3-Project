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
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

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

	UPROPERTY(EditAnywhere, Category = "Textures")
	class UInGameStateDataAsset* CurrentTextures;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStatus")
	int32 CurrentHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStatus")
	int32 MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStatus")
	int32 CurrentShield;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStatus")
	int32 MaxShield;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerState();
public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentHP(int32 UpdateHP, int32 UpdateMaxHP);
	UFUNCTION(BlueprintCallable)
	void SetCurrentShield(int32 UpdateShield, int32 UpdateMaxShield);

	UFUNCTION(BlueprintCallable)
	void SetCurrentPlayerTexture();
};
