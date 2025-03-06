// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AIObjectPool.h"
#include "Actor/BulletPool.h"
#include "Actor/Trap/DropManager.h"
#include "GameMode/ClearPortal.h"
#include "GameMode/TrapPortal.h"
#include "GameMode/CardData.h"
#include "Widgets/InGame/IngameSelectWidget.h"
#include "Widgets/InGame/IngameMainWidget.h"
#include "Widgets/Lobby/LobbyWidget.h"
#include "FPSGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Playing UMETA(DisplayName = "Playing"),
	CardSelection UMETA(DisplayName = "Card Selection")
};

UCLASS()
class GUNFIREPARAGON_API AFPSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	AFPSGameMode();
	virtual void BeginPlay() override;
	void InitializeObjectPool();
	void InitializeBulletPool();
	void TravelToLevel(FName LevelName);
	void ReturnToPreviousLevel();
	void SpawnTrapPortals();
	void SavePlayerLocation();
	void RestorePlayerLocation();
	void LoadCardDataFromDataTable();
	void ShowCardSelectionUI();
	void ContinueGameAfterCardSelection();
	void SetGameState(EGameState NewState);
	TMap<TSubclassOf<ABaseEnemy>, int32> GetPoolInitializationData();
	TMap<TSubclassOf<ABaseEnemy>, int32> GetEnemySpawnData(int32 StageNumber);
	UCardData* GetRandomCard();
	void InitializeDropManager();
	void PlayMainHudShow();
	ADropManager* GetDropManager();


	UFUNCTION(BlueprintCallable)
	TArray<UCardData*> GetRandomCards(int32 CardCount);
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
	UFUNCTION()
	ABulletPool* GetBulletPool(); 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropManager")
	ADropManager* DropManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropManager")
	TSubclassOf<ADropManager> DropManagerClass;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Table")
	UDataTable* CardDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card System")
	TArray<UCardData*> AllCardPool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clear Portal")
	TSubclassOf<AClearPortal> PortalClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clear Portal")
	TSubclassOf<ATrapPortal> TrapPortalClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	EGameState CurrentGameState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UIngameSelectWidget> CardSelectionWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UIngameMainWidget> PlayerMainHudClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	bool bIsObjectPoolReady;
	bool bIsInTrapLevel;
	bool bPortalSpawned;

	// ---------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	UIngameMainWidget* PlayerMainWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> EndingWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> DieAnimWidgetClass;

	void PlayEndingCredit();
	void PlayDieAnim();
};
