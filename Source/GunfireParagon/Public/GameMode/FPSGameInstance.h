// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AIObjectPool.h"
#include "Actor/Weapon/CGunBase.h"
#include "Widgets/InGame/IngameSelectWidget.h"
#include "Widgets/InGame/IngameMainWidget.h"
#include "Widgets/Lobby/LobbyWidget.h"
#include "FPSGameInstance.generated.h"

class APlayerCharacter;

UCLASS()
class GUNFIREPARAGON_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFPSGameInstance();
	virtual void Init() override;
	void SavePlayerStats(ACharacter* PlayerCharacter);
	void LoadPlayerStats();
	void SetPreviousLevel(const FString& LevelName);
	FString GetPreviousLevel() const;
	void SavePlayerLocation(const FVector& Location);
	FVector GetPlayerLocation() const;
	void SaveWeaponStats(APlayerCharacter* Player);
	void LoadWeaponStats(APlayerCharacter* Player);
	void PlayerLevelUp();

	bool bIsInTrapLevel = false;
	TSubclassOf<ACGunBase> PrimaryWeaponClass;
	TSubclassOf<ACGunBase> SecondaryWeaponClass;
	UIngameMainWidget* PlayerMainHud;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	int32 PlayerLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ExperiencePoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float MaxExp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float MaxShield = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float MaxHP = 100;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ShieldRegenRate = 5.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float ShieldRegenDelay = 5.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float DashCoolDown = 1.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Status")
	float NormalSpeed = 1000.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun Status")
	float GunDamage = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun Status")
	float GunDelay = 0.2f;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UIngameSelectWidget> CardSelectionWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UIngameMainWidget> PlayerMainHudClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;
	


	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void SaveMouseSensitivity();
	UFUNCTION(BlueprintCallable, Category = "Mouse")
	void LoadMouseSensitivity();
	UFUNCTION(BlueprintCallable, Category = "Level System")
	void AddExperiencePoint(float ExpAmount);
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void LoadNextStage();
	UFUNCTION(BlueprintCallable)
	void GotoMainMenu();

};
