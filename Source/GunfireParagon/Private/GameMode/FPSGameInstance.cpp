 // Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSGameInstance.h"
#include "GameMode/AIEnemySpawnRaw.h"
#include "GameMode/AIEnemyPoolRaw.h"
#include "GameMode/FPSGameMode.h"
#include "AI/NormalMeleeEnemy.h"
#include "AI/NormalRangeEnemy.h"
#include "Player/PlayerCharacter.h"
#include "Player/MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

UFPSGameInstance::UFPSGameInstance()
{
	MouseSensitivity = 
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	CurrentStageIndex = 0; 
	PreviousLevelName = TEXT("");
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
			SaveMouseSensitivity();
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


void UFPSGameInstance::SavePlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			//변수명 모름
			//PlayerLevel = Char->PlayerLevel;
			//ExperiencePoints = Char->CurrentExperience;
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
			//변수명 모름
			//Char->PlayerLevel = PlayerLevel;
			//Char->CurrentExperience = ExperiencePoints;
		}
	}
}

void UFPSGameInstance::SetPreviousLevel(const FString& LevelName)
{
	PreviousLevelName = LevelName;
}

FString UFPSGameInstance::GetPreviousLevel() const
{
	return PreviousLevelName;
}

void UFPSGameInstance::SavePlayerLocation(const FVector& Location)
{
	StoredPlayerLocation = Location;
}

FVector UFPSGameInstance::GetPlayerLocation() const
{
	return StoredPlayerLocation;
}



void UFPSGameInstance::SaveMouseSensitivity()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (FPSPlayerController)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FPSPlayerController->GetPawn());
			if (PlayerCharacter)
			{
				// MouseSensitivity = PlayerCharacter->MouseSensitivity or GetMouseSensitivity();
			}
		}
	}
}

void UFPSGameInstance::LoadMouseSensitivity()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (FPSPlayerController)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FPSPlayerController->GetPawn());
			if (PlayerCharacter)
			{
				// PlayerCharacter->MouseSensitivity = MouseSensitivity; || SetMouseSensitivity(MouseSensitivity);
			}
		}
	}
}

void UFPSGameInstance::AddExperiencePoint(float ExpAmount)
{
	ExperiencePoints += ExpAmount;

	//hile (PlayerLevel < ExpRequired.Num() - 1 && ExpRequired
}

void UFPSGameInstance::LevelUp()
{
}



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
