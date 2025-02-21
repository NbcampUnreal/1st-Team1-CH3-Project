// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AIObjectPool.h"
#include "FPSGameInstance.generated.h"


UCLASS()
class GUNFIREPARAGON_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFPSGameInstance();
	virtual void Init() override;
	void InitializeObjectPool();
	TMap<TSubclassOf<ABaseEnemy>, int32> GetEnemySpawnData(int32 StageNumber);
	TMap<TSubclassOf<ABaseEnemy>, int32> GetPoolInitializationData();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	int32 PlayerLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ExperiencePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	float PlayerHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	int32 CurrentStageIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<FName> StageMapNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Data")
	UDataTable* EnemySpawnTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Data")
	UDataTable* EnemyPoolTable;
	UPROPERTY()
	AAIObjectPool* AIObjectPoolInstance;

	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void LoadNextStage();
	UFUNCTION(BlueprintCallable)
	void GotoMainMenu();




	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PassiveInventory")
	// TArray<FPassiveInventory> PlayerPassiveInventory;

	// void SavePlayerStats(ACharacter* PlayerCharacter);
	// void LoadPlayerStats(ACharacter* PlayerCharacter);
	// void AddPassive(FPassiveInventory NewPassive);
	// void RemovePassive(FString PassiveName)
};
