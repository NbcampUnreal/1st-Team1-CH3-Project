// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"
#include "FPSGameState.h"
#include "FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../BaseEnemy.h"
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
	
	// HUD 추가 로직
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
	// 상자를 열면 다음스테이지 가던지, 어디에 도착하면 다음 스테이지 가던지 로직 구현해야함

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
					// 바로 스폰되면 어색할 수 있으니 보고 SetTimer 추가할지 생각
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
	

	// 스테이지 마다 스폰해야될 적, 적 개체수 가져옴
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

// Object Pool이기 때문에 필요 없지만 일단 놔둠
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

