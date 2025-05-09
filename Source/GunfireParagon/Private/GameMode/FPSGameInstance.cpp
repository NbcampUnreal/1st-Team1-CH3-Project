 // Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FPSGameInstance.h"
#include "GameMode/AIEnemySpawnRaw.h"
#include "GameMode/AIEnemyPoolRaw.h"
#include "GameMode/FPSGameMode.h"
#include "AI/Enemy/NormalMeleeEnemy.h"
#include "AI/Enemy/NormalRangeEnemy.h"
#include "Player/MyPlayerController.h"
#include "Player/PlayerCharacter.h"
#include "Actor/Weapon/CGunBase.h"
#include "Kismet/GameplayStatics.h"

UFPSGameInstance::UFPSGameInstance()
{
	StoredPlayerLocation = FVector::ZeroVector;
	MouseSensitivity = 1;
	PlayerLevel = 1;
	ExperiencePoints = 0.f;
	MaxExp = 100;
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

	FTimerHandle TestTimer;
	GetTimerManager().SetTimer(
		TestTimer,
		this,
		&UFPSGameInstance::LoadNextStage,
		3.0f,
		false
	);
	
}


void UFPSGameInstance::LoadNextStage()
{
	CurrentStageIndex++;

	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
			if (!FPSPlayerController || !FPSPlayerController->GetPawn()) return;

			StoredPlayerLocation = FPSPlayerController->GetPawn()->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Saved Player Location: %s"), *StoredPlayerLocation.ToString());

			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FPSPlayerController->GetPawn());

			if (PlayerCharacter)
			{
				SavePlayerStats(PlayerCharacter);
				SaveWeaponStats(PlayerCharacter);
				SaveMouseSensitivity();
			}
		}
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
	StartGame();
	
	/*
	CurrentStageIndex = 0;
	if (StageMapNames.IsValidIndex(CurrentStageIndex))
	{
		UGameplayStatics::OpenLevel(this, StageMapNames[CurrentStageIndex]);
	}
	*/
}


void UFPSGameInstance::SavePlayerStats(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
		if (Char)
		{
			MaxHP = Char->GetMaxHealth();
			MaxShield = Char->GetMaxShield();
			ShieldRegenDelay = Char->GetShieldRegenDelay();
			ShieldRegenRate = Char->GetShieldRegenRate();
			NormalSpeed = Char->GetNormalSpeed();
			DashCoolDown = Char->GetDashCoolDown();
		}
	}
}

void UFPSGameInstance::LoadPlayerStats()
{
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (FPSPlayerController)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FPSPlayerController->GetPawn());
			if (PlayerCharacter)
			{
				APlayerCharacter* Char = Cast<APlayerCharacter>(PlayerCharacter);
				if (Char)
				{
					Char->SetMaxHealth(MaxHP);
					Char->SetMaxShield(MaxShield);
					Char->SetShieldRegenDelay(ShieldRegenDelay);
					Char->SetShieldRegenRate(ShieldRegenRate);
					Char->SetNormalSpeed(NormalSpeed);
					Char->SetDashCoolDown(DashCoolDown);
				}
			}
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

void UFPSGameInstance::SaveWeaponStats(APlayerCharacter* Player)
{
	{
		if (!Player) return;

		if (Player->Inventory[0])
		{
			PrimaryWeaponClass = Player->Inventory[0]->GetClass();
			PrimaryGunDamage = Player->Inventory[0]->GetGunDamage();
			PrimaryDelay = Player->Inventory[0]->GetGunDelay();
			PrimaryAmmo = Player->Inventory[0]->GetMaxAmmo();
		}

		if (Player->Inventory[1])
		{
			SecondaryWeaponClass = Player->Inventory[1]->GetClass();
			SecondGunDamage = Player->Inventory[1]->GetGunDamage();
			SecondDelay = Player->Inventory[1]->GetGunDelay();
			SecondAmmo = Player->Inventory[1]->GetMaxAmmo();
		}
	}
}

void UFPSGameInstance::LoadWeaponStats(APlayerCharacter* Player)
{
	if (!Player) return;

	UWorld* World = Player->GetWorld();
	if (!World) return;

	if (PrimaryWeaponClass)
	{
		ACGunBase* NewPrimaryWeapon = World->SpawnActor<ACGunBase>(PrimaryWeaponClass);
		if (NewPrimaryWeapon)
		{
			Player->Inventory[0] = NewPrimaryWeapon;
			Player->CurrentWeapon = NewPrimaryWeapon;
			Player->CurrentWeaponSlot = 0;
			Player->AttachWeaponToHand(NewPrimaryWeapon, 0);
			Player->Inventory[0]->SetGunDamage(PrimaryGunDamage);
			Player->Inventory[0]->SetGunDelay(PrimaryDelay);
			Player->Inventory[0]->SetMaxAmmo(PrimaryAmmo);
		}
	}

	if (SecondaryWeaponClass)
	{
		ACGunBase* NewSecondaryWeapon = World->SpawnActor<ACGunBase>(SecondaryWeaponClass);
		if (NewSecondaryWeapon)
		{
			Player->Inventory[1] = NewSecondaryWeapon;
			Player->Inventory[1]->SetGunDamage(SecondGunDamage);
			Player->Inventory[1]->SetGunDelay(SecondDelay);
			Player->Inventory[1]->SetMaxAmmo(SecondAmmo);
		}
	}
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
				MouseSensitivity = PlayerCharacter->GetMouseSensitivity();
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
				PlayerCharacter->SetMouseSensitivity(MouseSensitivity);
			}
		}
	}
}

void UFPSGameInstance::AddExperiencePoint(float ExpAmount)
{
	ExperiencePoints += ExpAmount;
	UE_LOG(LogTemp, Warning, TEXT("Player Level: %d, Exp: %f / %f"), PlayerLevel, ExperiencePoints, MaxExp);

	if (ExperiencePoints >= MaxExp)
	{
		PlayerLevelUp();
		if (CardSelectionWidgetClass)
		{
			UIngameSelectWidget* CardSelectionWidget = CreateWidget<UIngameSelectWidget>(GetWorld(), CardSelectionWidgetClass);
			if (CardSelectionWidget)
			{
				CardSelectionWidget->AddToViewport();
				return;
			}
		}
		LoadPlayerStats();
	}
}

void UFPSGameInstance::PlayerLevelUp()
{
	int32 LevelsGained = FMath::FloorToInt32(ExperiencePoints / MaxExp);

	if (LevelsGained > 0) 
	{
		PlayerLevel += LevelsGained;
		ExperiencePoints = ExperiencePoints - (LevelsGained * MaxExp); 
	}

	MaxHP = 100 + (PlayerLevel * 10);
	MaxShield = 50 + (PlayerLevel * 5);
	MaxExp = ((PlayerLevel - 1) + 10) * 10;
}
