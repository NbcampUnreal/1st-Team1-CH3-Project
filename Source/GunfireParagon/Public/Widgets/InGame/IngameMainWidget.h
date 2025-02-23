// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UIngameMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:
	UPROPERTY(meta = (BindWidget))
	class UInGameMinimap* MinimapWidget;

	UPROPERTY(meta = (BindWidget))	
	class UIngamePlayerStatus* PlayerStatusWidget;

	UPROPERTY(meta = (BindWidget))
	class UIngameWeaponWidget* WeaponStatusWidget;

	UPROPERTY(meta = (BindWidget))
	class UIngameCrossHairs* CrossHairWidget;


protected:
	//PlayerStatus
	UFUNCTION()
	void OnPlayerHealthBinding(float CurrentHP, float MaxHP);

	UFUNCTION()
	void OnPlayerShieldBinding(float CurrentShield, float MaxShield);

};
