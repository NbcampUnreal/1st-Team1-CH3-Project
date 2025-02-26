// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/FPSGameMode.h"
#include "GameMode/FPSGameState.h"
#include "GameMode/FPSGameInstance.h"
#include "GameMode/AIObjectPool.h"
#include "GameMode/AIEnemyPoolRaw.h"
#include "GameMode/AIEnemySpawnRaw.h"
#include "GameMode/SpawnVolume.h"
#include "GameMode/ClearPortal.h"
#include "Engine/TargetPoint.h"
#include "Player/MyPlayerController.h"
#include "Player/PlayerCharacter.h"
#include "AI/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"


AFPSGameMode::AFPSGameMode()
{
	PlayerControllerClass = AMyPlayerController::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();

	
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();
	

	InitializeObjectPool();

	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (FPSGameInstance)
	{
		int32 CurrentStageIndex = FPSGameInstance->CurrentStageIndex;

		if (CurrentStageIndex <= 10)
		{
			SpawnEnemiesForStage(CurrentStageIndex);
		}
		else
		{
			OnBossDefeated();
		}
	}
	// HUD 
	/*
	AMyPlayerController* PlayerController = Cast<AmyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->ShowHUD();
	}
	*/
}

void AFPSGameMode::InitializeObjectPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(ObjectPoolInstance))
	{
		ObjectPoolInstance = World->SpawnActor<AAIObjectPool>();

		if (ObjectPoolInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Object Pool Created in Instance"));

			TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClassCount = GetPoolInitializationData();

			ObjectPoolInstance->InitializePool(EnemyClassCount);
		}
	}
}

TMap<TSubclassOf<ABaseEnemy>, int32> AFPSGameMode::GetPoolInitializationData()
{
	TMap<TSubclassOf<ABaseEnemy>, int32> PoolData;

	if (!AIDataTable) return PoolData;

	static const FString ContextString(TEXT("PoolInitializationContext"));
	TArray<FAIEnemyPoolRaw*> AllRows;
	AIDataTable->GetAllRows(ContextString, AllRows);
	UE_LOG(LogTemp, Warning, TEXT("Sucess DT"));

	for (FAIEnemyPoolRaw* Raw : AllRows)
	{
		if (Raw)
		{
			PoolData.Add(Raw->EnemyClass, Raw->InitEnemyCount);
		}
		UE_LOG(LogTemp, Warning, TEXT("Find Row"));
	}
	return PoolData;
}

void AFPSGameMode::SpawnEnemiesForStage(int32 StageNumber)
{
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (!FPSGameInstance) return;

	AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (!IsValid(FPSGameState)) return;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	if (FoundVolumes.Num() == 0) return;


	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData = GetEnemySpawnData(StageNumber);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found SpawnVolume"));
			for (const TPair<TSubclassOf<ABaseEnemy>, int32>& Pair : EnemyData)
			{
				TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
				int32 EnemyCount = Pair.Value;
				UE_LOG(LogTemp, Warning, TEXT("Get Pair Value: %d"), EnemyCount);
				for (int32 i = 0; i < EnemyCount; i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("Try Get Enemy :%s from Pool"), *EnemyClass->GetName());
					ABaseEnemy* SpawnedEnemy = ObjectPoolInstance->GetPooledAI(SpawnVolume, EnemyClass);
					
					if (SpawnedEnemy)
					{
						UE_LOG(LogTemp, Log, TEXT("Get Enemy from Pool Success"), EnemyCount);
						FPSGameState->RemainingEnemies++;
					}
				}
			}
		}
	}
}

TMap<TSubclassOf<ABaseEnemy>, int32> AFPSGameMode::GetEnemySpawnData(int32 StageNumber)
{
	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData;
	if (!EnemySpawnTable) return EnemyData;

	static const FString ContextString(TEXT("EnemySpawnContext"));
	TArray<FAIEnemySpawnRaw*> AllRows;
	EnemySpawnTable->GetAllRows(ContextString, AllRows);
	UE_LOG(LogTemp, Warning, TEXT("Sucess Get DTspawnData! AllRows.Num : %d"), AllRows.Num());

	for (FAIEnemySpawnRaw* Row : AllRows)
	{
		if (Row && Row->StageNumber == StageNumber)
		{
			if (EnemyData.Contains(Row->EnemyClass))
			{
				EnemyData[Row->EnemyClass] += Row->EnemyCount;
			}
			else
			{
				EnemyData.Add(Row->EnemyClass, Row->EnemyCount);
			}
		}
	}

	if (EnemyData.Num()>0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get SpawnRowdata Succeed"));
	}

	return EnemyData;
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
	SpawnPortal();
}

void AFPSGameMode::SpawnPortal()
{
	AActor* PortalSpawnPoint = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->GetName() == TEXT("PortalSpawnPoint"))
		{
			PortalSpawnPoint = Actor;
			break;
		}
	}

	if (PortalSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Spawn!"));

		GetWorld()->SpawnActor<AClearPortal>(
			PortalClass,
			PortalSpawnPoint->GetActorLocation(),
			FRotator::ZeroRotator);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Spawn Failed"));
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

