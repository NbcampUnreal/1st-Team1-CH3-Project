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

// AI ĳ�� ������ �̰� ȣ�� ���ֽø� �����ϰڽ��ϴ�.
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

