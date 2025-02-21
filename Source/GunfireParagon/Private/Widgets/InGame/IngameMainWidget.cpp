// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameMainWidget.h"
#include "Widgets\InGame\InGameMinimap.h"
#include "Widgets\InGame\IngamePlayerStatus.h"
#include "Widgets\InGame\IngameWeaponWidget.h"
#include "Widgets\InGame\IngameCrossHairs.h"

void UIngameMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatus Missing"));
	}
	if (!WeaponStatusWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Missing"));
	}
	if (!MinimapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Minimap Missing"));
	}
	if (!CrossHairWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CrossHair Missing"));
	}


}

void UIngameMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIngameMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
