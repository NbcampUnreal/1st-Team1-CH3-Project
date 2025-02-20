// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
class UButton;

UCLASS()
class GUNFIREPARAGON_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public :
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	UButton* InfoButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EndButton;

private:
	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnInfoButtonClicked();

	UFUNCTION()
	void OnEndButtonClicked();

};
