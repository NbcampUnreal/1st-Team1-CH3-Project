// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"


UFPSGameInstance::UFPSGameInstance()
{
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	PlayerHealth = 1000.0f;
	CurrentLevelIndex = 0;
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
	// 같은 패시브가 이미 있으면 중첩 증가
	for (FPassiveInventory& Passive : PlayerPassiveInventory)
	{
		if (Passive.PassiveName == NewPassive.PassiveName)
		{
			Passive.Quentity++;
			return;
		}
	}

	// 같은 패시브가 없으면 새 패시브로 추가
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
