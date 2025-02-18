// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "FPSGameMode.h"
#include "Kismet/GameplayStatics.h"

AFPSGameState::AFPSGameState()
{
	CurrentStage = 1;
	RemainingEnemies = 0;
}

// AI ĳ�� ������ �̰� ȣ�� ���ֽø� �����ϰڽ��ϴ�.
void AFPSGameState::OnEnemyKilled()
{
	RemainingEnemies--;
	UE_LOG(LogTemp, Warning, TEXT("Remaining Enemies : %d"), RemainingEnemies);

	if (RemainingEnemies <= 0)
	{
		AdvancedToNextStage();
	}
}

void AFPSGameState::AdvancedToNextStage()
{
	AFPSGameMode* GameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode) return;

	if (CurrentStage < 10)
	{
		CurrentStage++;
		UE_LOG(LogTemp, Warning, TEXT("Advance to stage : %d"), CurrentStage);
		// UGameplayStatics::OpenLevel(this, *Nextlevel); ���� ���� ����� �� �����ؾ� �ҵ�
		GameMode->StartNewStage(CurrentStage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss Defeated, Call GameMode OnBossDefeated(EndGame()) function"));
		GameMode->OnBossDefeated();
	}
}
