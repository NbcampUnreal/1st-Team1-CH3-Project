// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPSGameMode.generated.h"


UCLASS()
class GUNFIREPARAGON_API AFPSGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	AFPSGameMode();
	virtual void BeginPlay() override;
	void ClearAllEnemies();

	UFUNCTION()
	void OnBossDefeated();
	UFUNCTION()
	void OnPlayerDead();
	UFUNCTION()
	void OnStageClear();
	UFUNCTION()
	void SpawnEnemies(int32 NumEnemies);
	UFUNCTION()
	void EndGame(bool bPlayWin);
	UFUNCTION()
	void ReturnToMainMenu();
};
