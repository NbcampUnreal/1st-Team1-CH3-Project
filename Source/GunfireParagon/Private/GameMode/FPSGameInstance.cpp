 // Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSGameInstance.h"
#include "GameMode/AIEnemySpawnRaw.h"
#include "GameMode/AIEnemyPoolRaw.h"
#include "GameMode/FPSGameMode.h"
#include "AI/NormalMeleeEnemy.h"
#include "AI/NormalRangeEnemy.h"
#include "Kismet/GameplayStatics.h"

UFPSGameInstance::UFPSGameInstance()
{
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	PlayerHealth = 1000.0f;
	CurrentStageIndex = 0; 
}

void UFPSGameInstance::Init()
{
	Super::Init();
	StartGame();
}


void UFPSGameInstance::StartGame()
{
	CurrentStageIndex = 0;

	if (CurrentStageIndex == 0)
	{
		if (StageMapNames.IsValidIndex(CurrentStageIndex))
		{
			UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
		}
	}

	// Test용으로 일단 만듦. 5초뒤 1스테이지로 이동
	FTimerHandle TestTimer;
	GetTimerManager().SetTimer(
		TestTimer,
		this,
		&UFPSGameInstance::LoadNextStage,
		5.0f,
		false
	);
}


void UFPSGameInstance::LoadNextStage()
{
	CurrentStageIndex++;

	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (FPSGameMode)
		{
			FPSGameMode->ClearAllBullets();
			FPSGameMode->ClearAllEnemies();
		}

		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
		UE_LOG(LogTemp, Warning, TEXT("OpenStage : %s"), *StageMapNames[CurrentStageIndex].ToString());
	}
}

void UFPSGameInstance::GotoMainMenu()
{
	CurrentStageIndex = 0;
	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
}


/*
void UFPSGameInstance::SavePlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			PlayerLevel = Char->PlayerLevel;
			ExperiencePoints = Char->CurrentExperience;
			PlayerHealth = Char->CurrentHealth;
		}
	}
}

void UFPSGameInstance::LoadPlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			Char->PlayerLevel = PlayerLevel;
			Char->CurrentExperience = ExperiencePoints;
			Char->CurrentHealth = PlayerHealth;
		}
	}
}
*/


/*
void UFPSGameInstance::AddPassive(FPassiveInventory NewPassive)
{
	//
	for (FPassiveInventory& Passive : PlayerPassiveInventory)
	{
		if (Passive.PassiveName == NewPassive.PassiveName)
		{
			Passive.Quentity++;
			return;
		}
	}

	PlayerPassiveInventory.Add(NewPassive);
}

void UFPSGameInstance::RemovePassive(FString PassiveName)
{
	for (int32 i = 0; i < PlayerPassiveInventory.Num(); i++)
	{
		if (PlayerPassiveInventory[i].PassiveName == PassiveName)
		{
			PlayerPassiveInventory.RemoveAt(i);
			return;
		}
	}
}
*/
