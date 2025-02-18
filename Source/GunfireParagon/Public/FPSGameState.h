// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPSGameState.generated.h"


UCLASS()
class GUNFIREPARAGON_API AFPSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AFPSGameState();
	void OnEnemyKilled();
	void AdvancedToNextStage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	int32 CurrentStage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	int32 RemainingEnemies;
};
