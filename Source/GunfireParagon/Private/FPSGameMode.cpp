// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"
#include "FPSGameState.h"
#include "FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "AI/BaseEnemy.h"
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
	// ���ڸ� ���� ������������ ������, ��� �����ϸ� ���� �������� ������ ���� �����ؾ���

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GameInstance))
		{
			if (FPSGameInstance)
			{
				FPSGameInstance->LoadNextStage();

				int32 NewStageIndex = FPSGameInstance->CurrentStageIndex;

				if (NewStageIndex <= 10) 
				{
					// �ٷ� �����Ǹ� ����� �� ������ ���� SetTimer �߰����� ����
					SpawnEnemiesForStage(NewStageIndex);
				}
			}
		}
	}
}

void AFPSGameMode::SpawnEnemiesForStage(int32 StageNumber)
{
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (!FPSGameInstance) return;
	AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (!IsValid(FPSGameState)) return;
	AAIObjectPool* AIObjectPool = FPSGameInstance->AIObjectPoolInstance;
	if (!IsValid(AIObjectPool)) return;
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	if (FoundVolumes.Num() == 0) return;
	

	// �������� ���� �����ؾߵ� ��, �� ��ü�� ������
	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData = FPSGameInstance->GetEnemySpawnData(StageNumber); 

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			for (const TPair<TSubclassOf<ABaseEnemy>, int32>& Pair : EnemyData)
			{
				TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
				int32 EnemyCount = Pair.Value;

				for (int32 i = 0; i < EnemyCount; i++)
				{
					ABaseEnemy* SpawnedEnemy = AIObjectPool->GetPooledAI(SpawnVolume, EnemyClass);
					if (SpawnedEnemy)
					{
						FPSGameState->RemainingEnemies++;
					}
				}
			}
		}
	}

}

// Object Pool�̱� ������ �ʿ� ������ �ϴ� ����
/*
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
*/
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

