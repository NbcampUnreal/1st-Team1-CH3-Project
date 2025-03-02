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
	void SavePlayerStats(ACharacter* PlayerCharacter);
	void LoadPlayerStats(ACharacter* PlayerCharacter);
	void SetPreviousLevel(const FString& LevelName);
	FString GetPreviousLevel() const;
	void SavePlayerLocation(const FVector& Location);
	FVector GetPlayerLocation() const;
	

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	int32 PlayerLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ExperiencePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
	int32 CurrentStageIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<FName> StageMapNames;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TrapLevel")
	FString PreviousLevelName;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "TrapLevel")
	FVector StoredPlayerLocation;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Mouse")
	float MouseSensitivity;

	
	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void SaveMouseSensitivity();
	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void LoadMouseSensitivity();
	UFUNCTION(BlueprintCallable, Category = "Level System")
	void AddExperiencePoint(float ExpAmount);
	UFUNCTION(BlueprintCallable, Category = "Level System")
	void LevelUp();
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void LoadNextStage();
	UFUNCTION(BlueprintCallable)
	void GotoMainMenu();


	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PassiveInventory")
	// TArray<FPassiveInventory> PlayerPassiveInventory;


	// void AddPassive(FPassiveInventory NewPassive);
	// void RemovePassive(FString PassiveName)
};
