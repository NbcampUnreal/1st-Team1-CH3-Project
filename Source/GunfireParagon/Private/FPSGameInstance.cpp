// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"
#include "AIEnemySpawnRaw.h"
#include "AIEnemyPoolRaw.h"
#include "../NormalMeleeEnemy.h"
#include "../NormalRangeEnemy.h"
#include "Kismet/GameplayStatics.h"

UFPSGameInstance::UFPSGameInstance()
{
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	PlayerHealth = 1000.0f;
	CurrentStageIndex = 0; // 0 이 메인메뉴, 1 ~ 10 Stage 
	AIObjectPoolInstance = nullptr;
}

void UFPSGameInstance::Init()
{
	Super::Init();

	InitializeObjectPool();

	if (CurrentStageIndex == 0)
	{
		if (StageMapNames.IsValidIndex(CurrentStageIndex))
		{
			UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
		}
	}
}

void UFPSGameInstance::InitializeObjectPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(AIObjectPoolInstance)) // 생성된 Object Pool이 있는지 확인
	{
		AIObjectPoolInstance = World->SpawnActor<AAIObjectPool>(); // 없으면 생성 

		if (AIObjectPoolInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Object Pool Created in Instance"));

			 TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClassCount = GetPoolInitializationData();
			
			 /*
			 EnemyClassCount.Add(ANormalRangeEnemy::StaticClass(), 20); // 원거리 일반
			 EnemyClassCount.Add(ANormalMeleeEnemy::StaticClass(), 15); // 근거리 일반 스테이지 1 ~ 8
			 EnemyClassCount.Add(AEliteMeleeEnemy::StaticClass(), 25); // 근거리 정예 스테이지 3, 7 ~ 9
			 EnemyClassCount.Add(AEliteRangeEnemy::StaticClass(), 1); // 원거리 정예 스테이지 6
			 EnemyClassCount.Add(AEliteBufferEnemy::StaticClass(), 1); // 버퍼 정예 스테이지 9
			 EnemyClassCount.Add(ABossEnemy::StaticClass(), 1); // 보스 스테이지 10
			 */

			AIObjectPoolInstance->InitializePool(EnemyClassCount);
		}
	}
}

TMap<TSubclassOf<ABaseEnemy>, int32> UFPSGameInstance::GetEnemySpawnData(int32 StageNumber)
{
	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData;
	if (!EnemySpawnTable) return EnemyData; // 데이터 테이블 없으면 빈 EnemyData 반환

	static const FString ContextString(TEXT("EnemySpawnContext"));
	TArray<FAIEnemySpawnRaw*> AllRows;
	EnemySpawnTable->GetAllRows(ContextString, AllRows);

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

	return EnemyData;
}

TMap<TSubclassOf<ABaseEnemy>, int32> UFPSGameInstance::GetPoolInitializationData()
{
	TMap<TSubclassOf<ABaseEnemy>, int32> PoolData;

	if (!EnemyPoolTable) return PoolData;

	static const FString ContextString(TEXT("PoolInitializationContext"));
	TArray<FAIEnemyPoolRaw*> AllRows;
	EnemyPoolTable->GetAllRows(ContextString, AllRows);

	for (FAIEnemyPoolRaw* Raw : AllRows)
	{
		if (Raw)
		{
			PoolData.Add(Raw->EnemyClass, Raw->InitEnemyCount);
		}
	}
	return PoolData;
}

// LoadNextStage가 있어서 없어도 될 것 같긴한데 추후 유용할 수 있으니 남겨둠
void UFPSGameInstance::StartGame()
{
	CurrentStageIndex = 1;
	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
}

void UFPSGameInstance::LoadNextStage()
{
	CurrentStageIndex++;

	if (CurrentStageIndex > 10)
	{
		GotoMainMenu();
		return;
	}

	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
}

void UFPSGameInstance::GotoMainMenu()
{
	CurrentStageIndex = 0;
	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
}


/*
void UFPSGameInstance::SavePlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			PlayerLevel = Char->PlayerLevel;
			ExperiencePoints = Char->CurrentExperience;
			PlayerHealth = Char->CurrentHealth;
		}
	}
}

void UFPSGameInstance::LoadPlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			Char->PlayerLevel = PlayerLevel;
			Char->CurrentExperience = ExperiencePoints;
			Char->CurrentHealth = PlayerHealth;
		}
	}
}
*/


/*
void UFPSGameInstance::AddPassive(FPassiveInventory NewPassive)
{
	// 같은 패시브가 이미 있으면 중첩 증가
	for (FPassiveInventory& Passive : PlayerPassiveInventory)
	{
		if (Passive.PassiveName == NewPassive.PassiveName)
		{
			Passive.Quentity++;
			return;
		}
	}

	// 같은 패시브가 없으면 새 패시브로 추가
	PlayerPassiveInventory.Add(NewPassive);
}

void UFPSGameInstance::RemovePassive(FString PassiveName)
{
	for (int32 i = 0; i < PlayerPassiveInventory.Num(); i++)
	{
		if (PlayerPassiveInventory[i].PassiveName == PassiveName)
		{
			PlayerPassiveInventory.RemoveAt(i);
			return;
		}
	}
}
*/
