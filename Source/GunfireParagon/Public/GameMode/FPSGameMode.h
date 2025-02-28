// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AIObjectPool.h"
#include "Actor/BulletPool.h"
#include "GameMode/ClearPortal.h"
#include "FPSGameMode.generated.h"


UCLASS()
class GUNFIREPARAGON_API AFPSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	AFPSGameMode();
	virtual void BeginPlay() override;
	void InitializeObjectPool();
	void InitializeBulletPool();
	TMap<TSubclassOf<ABaseEnemy>, int32> GetPoolInitializationData();
	TMap<TSubclassOf<ABaseEnemy>, int32> GetEnemySpawnData(int32 StageNumber);


	UFUNCTION()
	void OnBossDefeated();
	UFUNCTION()
	void OnPlayerDead();
	UFUNCTION()
	void OnStageClear();
	UFUNCTION()
	void SpawnPortal();
	UFUNCTION()
	void SpawnEnemiesForStage(int32 StageNumber);
	UFUNCTION()
	void ClearAllEnemies();
	UFUNCTION()
	void ClearAllBullets();
	UFUNCTION()
	void EndGame(bool bPlayWin);
	UFUNCTION()
	void ReturnToMainMenu();
	
	UPROPERTY()
	AAIObjectPool* ObjectPoolInstance;
	UPROPERTY()
	ABulletPool* BulletPoolInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pool")
	TSubclassOf<ABulletPool> BulletPoolClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Table")
	UDataTable* AIDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Table")
	UDataTable* EnemySpawnTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clear Portal")
	TSubclassOf<AClearPortal> PortalClass;

	bool bIsObjectPoolReady;
};
