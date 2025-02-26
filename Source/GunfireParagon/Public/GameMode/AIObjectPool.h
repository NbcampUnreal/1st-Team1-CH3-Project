// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"
#include "AIObjectPool.generated.h"

class ABaseEnemy;
class ASpawnVolume;

UCLASS()
class GUNFIREPARAGON_API AAIObjectPool : public AActor
{
	GENERATED_BODY()
	
public:	
	

	AAIObjectPool();
	virtual void BeginPlay() override;

	void InitializePool(TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClasses);
	ABaseEnemy* GetPooledAI(ASpawnVolume* SpawnVolume, TSubclassOf<ABaseEnemy> EnemyClass);
	void ReturnAIToPool(ABaseEnemy* Enemy);

	TMap<TSubclassOf<ABaseEnemy>, TSharedPtr<TArray<ABaseEnemy*>>> EnemyPools;
	TSharedPtr<TArray<ABaseEnemy*>> Pool;


private:
	
};
