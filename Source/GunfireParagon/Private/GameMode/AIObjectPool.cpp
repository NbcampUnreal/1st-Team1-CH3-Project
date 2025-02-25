// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/AIObjectPool.h"
#include "GameMode/SpawnVolume.h"
#include "AI/BaseEnemy.h"
#include "GameMode/SpawnVolume.h"
#include "Engine/World.h"
#include "AI/NormalMeleeEnemy.h"


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
	for (const TPair<TSubclassOf<ABaseEnemy>, int32> Pair : EnemyClasses)
	{
		TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
		int32 Count = Pair.Value;
		
		if (!EnemyClass) continue;
			
		Pool = EnemyPools.FindOrAdd(EnemyClass);
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		for (int32 i = 0; i < Count; i++)
		{
			ABaseEnemy* NewEnemy = GetWorld()->SpawnActor<ABaseEnemy>(
				EnemyClass,
				SpawnParams
				);
		
			if (NewEnemy->IsValidLowLevelFast())
			{
				NewEnemy->SetActorHiddenInGame(true);
				NewEnemy->SetActorEnableCollision(false);
				NewEnemy->SetActorTickEnabled(false);
				UE_LOG(LogTemp, Warning, TEXT("add"));
				if (Pool.IsValid())
				{
					Pool->Add(NewEnemy);
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
		Pool = EnemyPools[EnemyClass];

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
		Enemy->SetActorHiddenInGame(false);
		Enemy->SetActorEnableCollision(false);
		Enemy->SetActorTickEnabled(false);
	}
}


