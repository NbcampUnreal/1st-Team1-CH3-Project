// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPSGameInstance.generated.h"


UCLASS()
class GUNFIREPARAGON_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFPSGameInstance();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	int32 PlayerLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ExperiencePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Status")
	float PlayerHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 CurrentLevelIndex;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PassiveInventory")
	// TArray<FPassiveInventory> PlayerPassiveInventory;

	// void SavePlayerStats(ACharacter* PlayerCharacter);
	// void LoadPlayerStats(ACharacter* PlayerCharacter);
	// void AddPassive(FPassiveInventory NewPassive);
	// void RemovePassive(FString PassiveName)
};
