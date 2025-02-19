// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"
#include "FPSGameState.h"
#include "FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "BaseEnemy.h"
#include "AIObjectPool.h"
#include "SpawnVolume.h"

AFPSGameMode::AFPSGameMode()
{
	// DefaultPawnClass = APlayerCharacter::StaticClass();
	// PlayerControllerClass = AMyPlayerController::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// HUD �߰� ����
	/*
	AMyPlayerController* PlayerController = Cast<AmyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->ShowHUD();
	}
	*/
}

void AFPSGameMode::OnBossDefeated()
{
	EndGame(true);
}

void AFPSGameMode::OnPlayerDead()
{
	EndGame(false);
}

void AFPSGameMode::OnStageClear()
{
	ClearAllEnemies();

	// ���ڸ� ���� ������������ ������, ��� �����ϸ� ���� �������� ������ ���� �����ؾ���

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GameInstance))
		{
			if (FPSGameInstance)
			{
				FPSGameInstance->LoadNextStage();

				int32 NewStageIndex = FPSGameInstance->CurrentStageIndex;
				int32 NumEnemies = NewStageIndex * 5;

				// �ٷ� �����Ǹ� ����� �� ������ ���� SetTimer �߰����� ����
				SpawnEnemies(NumEnemies);
			}
		}
	}
}

void AFPSGameMode::SpawnEnemies(int32 NumEnemies)
{
	UWorld* World = GetWorld();
	if (!World) return;

	AFPSGameState* FPSGameState = Cast<AFPSGameState>(World->GetGameState());
	if (!FPSGameState) return;

	AAIObjectPool* AIObjectPool = Cast<AAIObjectPool>(UGameplayStatics::GetActorOfClass(World, AAIObjectPool::StaticClass()));
	if (AIObjectPool) return;

	TArray<AActor*> FoundVolumes;

	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundVolumes);
	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (int32 i = 0; i < NumEnemies; i++)
			{
				ABaseEnemy* SpawnedEnemy = AIObjectPool->GetPooledAI(SpawnVolume);
				if (SpawnedEnemy)
				{
					FPSGameState->RemainingEnemies++;
				}
			}
		}	
	}
}

void AFPSGameMode::ClearAllEnemies()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, ABaseEnemy::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
		}
	}
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
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GameInstance))
		{
			if (FPSGameInstance)
			{
				FPSGameInstance->GotoMainMenu();
			}
		}
	}	
}

