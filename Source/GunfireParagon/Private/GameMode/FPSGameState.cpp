// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSGameState.h"
#include "GameMode/FPSGameMode.h"
#include "GameMode/FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"

AFPSGameState::AFPSGameState()
{
	CurrentStageIndex = 1;
	RemainingEnemies = 0;
}

void AFPSGameState::OnEnemyKilled()
{
	RemainingEnemies--;
	UE_LOG(LogTemp, Warning, TEXT("Remaining Enemies : %d"), RemainingEnemies);
	
	if (RemainingEnemies <= 0)
	{
		if (AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (FPSGameMode)
			{
				FPSGameMode->OnStageClear();	
			}
		}
	}
}

