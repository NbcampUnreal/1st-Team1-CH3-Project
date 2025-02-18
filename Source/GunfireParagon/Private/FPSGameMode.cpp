// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"
#include "FPSGameState.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// DefaultPawnClass = APlayerCharacter::StaticClass();
	// PlayerControllerClass = AMyPlayerController::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::StartPlay()
{
	Super::StartPlay();

}

void AFPSGameMode::OnBossDefeated()
{
	EndGame(true);
}

void AFPSGameMode::OnPlayerDead()
{
	EndGame(false);
}

void AFPSGameMode::EndGame(bool bPlayWin)
{
	if (bPlayWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));
		// ���� Ŭ���� UI ȣ���ϴ� ���� �߰��ؾ߉�.
		FTimerHandle EndTimerHandle;
		GetWorldTimerManager().SetTimer(
			EndTimerHandle,
			this,
			&AFPSGameMode::ReturnToMainMenu,
			5.0f,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
		// ���� ���� UI ȣ���ϴ� ���� �߰��ؾ߉�.
		FTimerHandle EndTimerHandle;
		GetWorldTimerManager().SetTimer(
			EndTimerHandle,
			this,
			&AFPSGameMode::ReturnToMainMenu,
			5.0f,
			false
		);
	}
}

void AFPSGameMode::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(this, "MainMenu");
}

void AFPSGameMode::StartNewStage(int32 Stage)
{
	AFPSGameState* FPSGameState = GetWorld()->GetGameState<AFPSGameState>();
	if (!FPSGameState) return;

	FPSGameState->RemainingEnemies = Stage * 10; // �������� * 10������ ����
	UE_LOG(LogTemp, Warning, TEXT("Stage : %d, SpawnEnemies : %d"), Stage, FPSGameState->RemainingEnemies);

	// ĳ����, ��AI, ���� ���� ���� �ʿ���
}
