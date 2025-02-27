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

	UPROPERTY(meta = (BindWidget))
	class UDashEffectWidget* DashWidget;
protected:
	//Player Status
	UFUNCTION()
	void OnPlayerHealthBinding(float CurrentHP, float MaxHP);

	UFUNCTION()
	void OnPlayerShieldBinding(float CurrentShield, float MaxShield);

	UFUNCTION()
	void OnPlayerIsDashBinding(bool IsDash);

	//Weapon Status
	UFUNCTION()
	void OnWeaponAmmoBinding(float CurrentAmmo, float MaxAmmo);

	//Minimap RenderIcon Update
	UFUNCTION()
	void OnMinimapUpdated(ACharacter* Target, float Distance);

private:
	// Minimap Rendering Character Distance
	float MaxRenderDistance = 1600.f;
};
