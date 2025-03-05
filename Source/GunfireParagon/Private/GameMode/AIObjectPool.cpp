// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/AIObjectPool.h"
#include "GameMode/SpawnVolume.h"
#include "AI/BaseEnemy.h"
#include "GameMode/SpawnVolume.h"
#include "Engine/World.h"
#include "AI/NormalMeleeEnemy.h"
#include "AI/BaseEnemy.h"


AAIObjectPool::AAIObjectPool()
{
	PrimaryActorTick.bCanEverTick = false;
	Pool = MakeShared<TArray<ABaseEnemy*>>();
}

void AAIObjectPool::BeginPlay()
{
	Super::BeginPlay();
}


void AAIObjectPool::InitializePool(TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClasses)
{
	EnemyPools.Empty();

	for (const TPair<TSubclassOf<ABaseEnemy>, int32> Pair : EnemyClasses)
	{
		TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
		int32 Count = Pair.Value;
		
		if (!EnemyClass) continue;
			
		Pool = EnemyPools.FindOrAdd(EnemyClass);
		if (!Pool.IsValid())
		{
			Pool = MakeShared<TArray<ABaseEnemy*>>();
			EnemyPools.Add(EnemyClass, Pool);
		}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		for (int32 i = 0; i < Count; i++)
		{
			ABaseEnemy* NewEnemy = GetWorld()->SpawnActor<ABaseEnemy>(
				EnemyClass,
				SpawnParams
				);
		
			if (NewEnemy->IsValidLowLevelFast())
			{
				NewEnemy->SetCanAttack(false);
				NewEnemy->SetActorHiddenInGame(true);
				NewEnemy->SetActorEnableCollision(false);
				NewEnemy->SetActorTickEnabled(false);

				if (Pool.IsValid())
				{
					Pool->Add(NewEnemy);
					UE_LOG(LogTemp, Warning, TEXT("Pool Save Data %d"), Pool->Num())
				}
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Created %d enemies of type!!!: %s"), i, *EnemyClass->GetName());
		}
	}
}

ABaseEnemy* AAIObjectPool::GetPooledAI(ASpawnVolume* SpawnVolume, TSubclassOf<ABaseEnemy> EnemyClass)
{
	if (!SpawnVolume || !EnemyClass) return nullptr;

	if (EnemyPools.Contains(EnemyClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyPools Contains BaseEnemyClass"));
		
		Pool = EnemyPools[EnemyClass];

		if (Pool.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Pool Is Valid"));	
			for (ABaseEnemy* Enemy : *Pool)
			{
				if (!Enemy->IsActorTickEnabled())
				{
					FVector SpawnLocation = SpawnVolume->GetSafeSpawnPoint();
					Enemy->SetActorLocation(SpawnLocation);
					Enemy->SetCanAttack(true);
					Enemy->ResetEnemy();
					UE_LOG(LogTemp, Log, TEXT("%s spawned from pool"), *EnemyClass->GetName());
					return Enemy;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pool is Nullptr"));
		}
	}
	return nullptr;
}

void AAIObjectPool::ReturnAIToPool(ABaseEnemy* Enemy)
{
	if (Enemy)
	{
		Enemy->SetActorHiddenInGame(false);
		Enemy->SetActorEnableCollision(false);
		Enemy->SetActorTickEnabled(false);
	}
}


