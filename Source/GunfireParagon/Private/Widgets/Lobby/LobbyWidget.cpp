// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/LobbyWidget.h"
#include "Components\Button.h"

void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (StartButton && !StartButton->OnClicked.IsBound())
	{
		StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartButtonClicked);
	}

	if (InfoButton && !InfoButton->OnClicked.IsBound())
	{
		InfoButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnInfoButtonClicked);
	}

	if (EndButton && !EndButton->OnClicked.IsBound())
	{
		EndButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnEndButtonClicked);
	}

}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULobbyWidget::OnStartButtonClicked()
{
	// GameMode->OpenLevel();
	// RemoveFromParent();
	UE_LOG(LogTemp, Display, TEXT("StartButton Binding Test"));
}

void ULobbyWidget::OnInfoButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("InfoButton Binding Test"));
}

void ULobbyWidget::OnEndButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("EndButton Binding Test"));
}
