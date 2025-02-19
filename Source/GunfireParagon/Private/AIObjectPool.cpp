// Fill out your copyright notice in the Description page of Project Settings.


#include "AIObjectPool.h"
#include "SpawnVolume.h"
#include "BaseEnemy.h" 


AAIObjectPool::AAIObjectPool()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AAIObjectPool::InitializePool(int32 PoolSize, TSubclassOf<ABaseEnemy> EnemyClass)
{
	EnemyBP = EnemyClass;

	for (int32 i = 0; i < PoolSize; i++)
	{
		ABaseEnemy* NewEnemy = GetWorld()->SpawnActor<ABaseEnemy>(
			EnemyBP,
			FVector::ZeroVector,
			FRotator::ZeroRotator);

		// 미리 Spawn, 스크럼때 얘기했던 Object Pooling 방식
		if (NewEnemy)
		{
			NewEnemy->SetActorHiddenInGame(true);
			NewEnemy->SetActorEnableCollision(false);
			NewEnemy->SetActorTickEnabled(false);
			PooledEnemies.Add(NewEnemy);
		}
	}
}

ABaseEnemy* AAIObjectPool::GetPooledAI(ASpawnVolume* SpawnVolume)
{
	if (!SpawnVolume) return nullptr;

	for (ABaseEnemy* Enemy : PooledEnemies)
	{
		if (!Enemy->IsActorTickEnabled())
		{
			FVector SpawnLocation = SpawnVolume->GetSafeSpawnPoint();
			Enemy->SetActorLocation(SpawnLocation);
			Enemy->SetActorHiddenInGame(false);
			Enemy->SetActorEnableCollision(true);
			Enemy->SetActorTickEnabled(true);
			return Enemy;
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


