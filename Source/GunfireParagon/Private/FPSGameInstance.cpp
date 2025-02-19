// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"
#include "Kismet/GameplayStatics.h"

UFPSGameInstance::UFPSGameInstance()
{
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	PlayerHealth = 1000.0f;
	CurrentStageIndex = 0; // 0 �� ���θ޴�, 1 ~ 10 Stage 
}

void UFPSGameInstance::Init()
{
	Super::Init();

	if (CurrentStageIndex == 0)
	{
		if (StageMapNames.IsValidIndex(CurrentStageIndex))
		{
			UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
		}
	}
}

// LoadNextStage�� �־ ��� �� �� �����ѵ� ���� ������ �� ������ ���ܵ�
void UFPSGameInstance::StartGame()
{
	CurrentStageIndex = 1;
	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
}

void UFPSGameInstance::LoadNextStage()
{
	CurrentStageIndex++;

	if (CurrentStageIndex > 10)
	{
		GotoMainMenu();
		return;
	}

	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
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
	// ���� �нú갡 �̹� ������ ��ø ����
	for (FPassiveInventory& Passive : PlayerPassiveInventory)
	{
		if (Passive.PassiveName == NewPassive.PassiveName)
		{
			Passive.Quentity++;
			return;
		}
	}

	// ���� �нú갡 ������ �� �нú�� �߰�
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
