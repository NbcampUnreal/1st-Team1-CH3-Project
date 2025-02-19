// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIObjectPool.generated.h"

class ABaseEnemy; // µ¿Çö´Ô Ã¼Å©ÇØÁÖ½Ê¼î!
class ASpawnVolume;

UCLASS()
class GUNFIREPARAGON_API AAIObjectPool : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAIObjectPool();

	void InitializePool(int32 PoolSize, TSubclassOf<ABaseEnemy> EnemyClass);
	ABaseEnemy* GetPooledAI(ASpawnVolume* SpawnVolume);
	void ReturnAIToPool(ABaseEnemy* Enemy);

	UPROPERTY()
	TArray<ABaseEnemy*> PooledEnemies;
	UPROPERTY()
	TSubclassOf<ABaseEnemy> EnemyBP;
};
