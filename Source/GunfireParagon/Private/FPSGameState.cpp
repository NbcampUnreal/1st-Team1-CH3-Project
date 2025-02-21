// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "FPSGameMode.h"
#include "FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"

AFPSGameState::AFPSGameState()
{
	CurrentStageIndex = 1;
	RemainingEnemies = 0;
}

// AI 캐릭 죽으면 이거 호출 해주시면 감사하겠습니다.
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

