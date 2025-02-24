// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameMinimapIcon.h"
#include "Kismet\GameplayStatics.h"

void UIngameMinimapIcon::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateIconPosition();
}

void UIngameMinimapIcon::SetWorldLocation(const FVector& NewLocation)
{
	WorldLocation = NewLocation;
	UpdateIconPosition();
}

void UIngameMinimapIcon::UpdateIconPosition()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;
}
