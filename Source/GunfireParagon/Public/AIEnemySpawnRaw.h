// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../BaseEnemy.h"
#include "AIEnemySpawnRaw.generated.h"

USTRUCT(BlueprintType)
struct FAIEnemySpawnRaw : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIData")
	FName EnemyName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIData")
	TSubclassOf<ABaseEnemy> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIData")
	int32 EnemyCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIData")
	int32 StageNumber;
};
