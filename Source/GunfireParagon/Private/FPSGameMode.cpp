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
		// 게임 클리어 UI 호출하는 로직 추가해야됌.
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
		// 게임 오버 UI 호출하는 로직 추가해야됌.
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

	FPSGameState->RemainingEnemies = Stage * 10; // 스테이지 * 10마리로 설정
	UE_LOG(LogTemp, Warning, TEXT("Stage : %d, SpawnEnemies : %d"), Stage, FPSGameState->RemainingEnemies);

	// 캐릭터, 적AI, 액터 스폰 로직 필요함
}
