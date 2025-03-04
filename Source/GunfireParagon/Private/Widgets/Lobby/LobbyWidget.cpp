// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/LobbyWidget.h"
#include "Kismet\GameplayStatics.h"
#include "Player\MyPlayerController.h"

void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
		}
		
	}

	SetIsFocusable(true);
}

FReply ULobbyWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
		RemoveFromParent();
	}

	SetIsFocusable(false);
	return FReply::Handled();
}
