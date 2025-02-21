// Fill out your copyright notice in the Description page of Project Settings.


#include "AIObjectPool.h"
#include "SpawnVolume.h"
#include "AI/BaseEnemy.h" 


AAIObjectPool::AAIObjectPool()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AAIObjectPool::InitializePool(TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClasses)
{
	for (const TPair<TSubclassOf<ABaseEnemy>, int32>& Pair : EnemyClasses)
	{
		TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
		int32 Count = Pair.Value;
		
		if (!EnemyClass) continue;

		TSharedPtr<TArray<ABaseEnemy*>>& Pool = EnemyPools.FindOrAdd(EnemyClass);
		if (!Pool.IsValid())  // Pool�� ���ٸ� ���� ����
		{
			Pool = MakeShared<TArray<ABaseEnemy*>>();
		}

		for (int32 i = 0; i < Count; i++)
		{
			ABaseEnemy* NewEnemy = GetWorld()->SpawnActor<ABaseEnemy>(
				EnemyClass,
				FVector::ZeroVector,
				FRotator::ZeroRotator);

			if (NewEnemy)
			{
				NewEnemy->SetActorHiddenInGame(true);
				NewEnemy->SetActorEnableCollision(false);
				NewEnemy->SetActorTickEnabled(false);
				if (Pool.IsValid())
				{
					Pool->Add(NewEnemy);
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Created %d enemies of type: %s"), Count, *EnemyClass->GetName());
	}
}

ABaseEnemy* AAIObjectPool::GetPooledAI(ASpawnVolume* SpawnVolume, TSubclassOf<ABaseEnemy> EnemyClass)
{
	if (!SpawnVolume || !EnemyClass) return nullptr;

	if (EnemyPools.Contains(EnemyClass))
	{
		TSharedPtr<TArray<ABaseEnemy*>>& Pool = EnemyPools[EnemyClass];

		if (Pool.IsValid())
		{
			for (ABaseEnemy* Enemy : *Pool)
			{
				if (!Enemy->IsActorTickEnabled())
				{
					FVector SpawnLocation = SpawnVolume->GetSafeSpawnPoint();
					Enemy->SetActorLocation(SpawnLocation);
					Enemy->SetActorHiddenInGame(false);
					Enemy->SetActorEnableCollision(true);
					Enemy->SetActorTickEnabled(true);

					UE_LOG(LogTemp, Log, TEXT("%s spawned from pool"), *EnemyClass->GetName());
					return Enemy;
				}
			}
		}
	}
	return nullptr;
}

void AAIObjectPool::ReturnAIToPool(ABaseEnemy* Enemy)
{
	if (Enemy)
	{
		Enemy->SetActorHiddenInGame(true);
		Enemy->SetActorEnableCollision(false);
		Enemy->SetActorTickEnabled(false);
	}
}


