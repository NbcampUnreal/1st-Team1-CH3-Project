// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/FPSGameMode.h"
#include "GameMode/FPSGameState.h"
#include "GameMode/FPSGameInstance.h"
#include "GameMode/AIObjectPool.h"
#include "GameMode/AIEnemyPoolRaw.h"
#include "GameMode/AIEnemySpawnRaw.h"
#include "GameMode/SpawnVolume.h"
#include "GameMode/ClearPortalPoint.h"
#include "GameMode/TrapPortalPoint.h"
#include "Engine/TargetPoint.h"
#include "Blueprint/UserWidget.h"
#include "Player/MyPlayerController.h"
#include "Player/PlayerCharacter.h"
#include "AI/BaseEnemy.h"
#include "Actor/Bullet/BulletBase.h"
#include "Kismet/GameplayStatics.h"


AFPSGameMode::AFPSGameMode()	
{
	bUseSeamlessTravel = true;
	bPortalSpawned = false;

	PlayerControllerClass = AMyPlayerController::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::BeginPlay()
{	
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay() Called"));

	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (FPSGameInstance)
	{
		if (FPSGameInstance->CurrentStageIndex == 0)
		{
			ULobbyWidget* LobbyWidget = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass);
			if (LobbyWidget)
			{
				LobbyWidget->AddToViewport();
			}
		}
		FString PreviousLevel = FPSGameInstance->GetPreviousLevel();
		bIsInTrapLevel = FPSGameInstance->bIsInTrapLevel;
		if (bIsInTrapLevel)
		{
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay() - Returning from TrapLevel, Restoring Player Location."));
			RestorePlayerLocation();
			FPSGameInstance->bIsInTrapLevel = false;

			if (CardSelectionWidgetClass)
			{
				UIngameSelectWidget* CardSelectionWidget = CreateWidget<UIngameSelectWidget>(GetWorld(), CardSelectionWidgetClass);
				if (CardSelectionWidget)
				{
					CardSelectionWidget->AddToViewport();
				}
			}
		}
		else
		{
			FPSGameInstance->bIsInTrapLevel = false;
			UE_LOG(LogTemp, Warning, TEXT("BeginPlay() - Normal level transition, skipping RestorePlayerLocation()."));
			SpawnTrapPortals();
		}

	}
	
	if (FPSGameInstance->CurrentStageIndex != 0)
	{
		FTimerHandle HudTimer;
		GetWorld()->GetTimerManager().SetTimer(
			HudTimer,
			this,
			&AFPSGameMode::PlayMainHudShow,
			0.5,
			false
		);
	}
	

	LoadCardDataFromDataTable();
	InitializeObjectPool();
	InitializeBulletPool();
	InitializeDropManager();

	if (FPSGameInstance)
	{
		int32 CurrentStageIndex = FPSGameInstance->CurrentStageIndex;

		if (CurrentStageIndex <= 10)
		{
			SpawnEnemiesForStage(CurrentStageIndex);
		}
		else
		{
			OnBossDefeated();
		}
	}
}


void AFPSGameMode::InitializeObjectPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(ObjectPoolInstance))
	{
		ObjectPoolInstance = World->SpawnActor<AAIObjectPool>();

		if (ObjectPoolInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Object Pool Created in Instance"));

			TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClassCount = GetPoolInitializationData();

			ObjectPoolInstance->InitializePool(EnemyClassCount);
		}
	}
}

void AFPSGameMode::InitializeBulletPool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (BulletPoolInstance)
	{
		return;
	}

	if (IsValid(BulletPoolClass))
	{
		BulletPoolInstance = World->SpawnActor<ABulletPool>(BulletPoolClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (BulletPoolInstance)
		{
			BulletPoolInstance->InitializePooled();
		}
	}
}


TMap<TSubclassOf<ABaseEnemy>, int32> AFPSGameMode::GetPoolInitializationData()
{
	TMap<TSubclassOf<ABaseEnemy>, int32> PoolData;

	if (!AIDataTable) return PoolData;

	static const FString ContextString(TEXT("PoolInitializationContext"));
	TArray<FAIEnemyPoolRaw*> AllRows;
	AIDataTable->GetAllRows(ContextString, AllRows);
	UE_LOG(LogTemp, Warning, TEXT("Sucess DT"));

	for (FAIEnemyPoolRaw* Raw : AllRows)
	{
		if (Raw)
		{
			PoolData.Add(Raw->EnemyClass, Raw->InitEnemyCount);
		}
		UE_LOG(LogTemp, Warning, TEXT("Find Row"));
	}
	return PoolData;
}

void AFPSGameMode::SpawnEnemiesForStage(int32 StageNumber)
{
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (!FPSGameInstance) return;

	AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (!IsValid(FPSGameState)) return;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	if (FoundVolumes.Num() == 0) return;


	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData = GetEnemySpawnData(StageNumber);

	if (FoundVolumes.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found %d SpawnVolumes"), FoundVolumes.Num());

		for (const TPair<TSubclassOf<ABaseEnemy>, int32>& Pair : EnemyData)
		{
			TSubclassOf<ABaseEnemy> EnemyClass = Pair.Key;
			int32 EnemyCount = Pair.Value;
			UE_LOG(LogTemp, Warning, TEXT("Get Pair Value: %d"), EnemyCount);

			for (int32 i = 0; i < EnemyCount; i++)
			{
				int32 RandomIndex = FMath::RandRange(0, FoundVolumes.Num() - 1);
				ASpawnVolume* SelectedSpawnVolume = Cast<ASpawnVolume>(FoundVolumes[RandomIndex]);

				if (SelectedSpawnVolume)
				{
					UE_LOG(LogTemp, Warning, TEXT("Using SpawnVolume at index: %d"), RandomIndex);
					UE_LOG(LogTemp, Warning, TEXT("Try Get Enemy :%s from Pool"), *EnemyClass->GetName());

					ABaseEnemy* SpawnedEnemy = ObjectPoolInstance->GetPooledAI(SelectedSpawnVolume, EnemyClass);

					if (SpawnedEnemy)
					{
						UE_LOG(LogTemp, Warning, TEXT("Get Enemy from Pool Success"));
						FPSGameState->RemainingEnemies++;
					}
				}
			}
		}
	}
}

TMap<TSubclassOf<ABaseEnemy>, int32> AFPSGameMode::GetEnemySpawnData(int32 StageNumber)
{
	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyData;
	if (!EnemySpawnTable) return EnemyData;

	static const FString ContextString(TEXT("EnemySpawnContext"));
	TArray<FAIEnemySpawnRaw*> AllRows;
	EnemySpawnTable->GetAllRows(ContextString, AllRows);
	UE_LOG(LogTemp, Warning, TEXT("Sucess Get DTspawnData! AllRows.Num : %d"), AllRows.Num());

	for (FAIEnemySpawnRaw* Row : AllRows)
	{
		if (Row && Row->StageNumber == StageNumber)
		{
			if (EnemyData.Contains(Row->EnemyClass))
			{
				EnemyData[Row->EnemyClass] += Row->EnemyCount;
			}
			else
			{
				EnemyData.Add(Row->EnemyClass, Row->EnemyCount);
			}
		}
	}

	if (EnemyData.Num()>0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get SpawnRowdata Succeed"));
	}

	return EnemyData;
}

void AFPSGameMode::OnBossDefeated()
{
	EndGame(true);
}

void AFPSGameMode::OnPlayerDead()
{
	EndGame(false);
}

void AFPSGameMode::OnStageClear()
{
	/*
	if (CardSelectionWidgetClass)
	{
		UIngameSelectWidget* CardSelectionWidget = CreateWidget<UIngameSelectWidget>(GetWorld(), CardSelectionWidgetClass);
		if (CardSelectionWidget)
		{
			CardSelectionWidget->AddToViewport();
		}
	}
	*/
	if (bPortalSpawned) return;
	
	SpawnPortal();
}

void AFPSGameMode::SpawnPortal()
{
	bPortalSpawned = true;

	AActor* PortalSpawnPoint = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClearPortalPoint::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Warning, TEXT("Found %d PortalSpawnPoints"), FoundActors.Num());

	for (AActor* Actor : FoundActors)
	{
		if (Actor && Actor->ActorHasTag(FName("ClearPortalPoint")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Actor: %s"), *Actor->GetFName().ToString());
			PortalSpawnPoint = Actor;
			break;
		}
	}

	if (PortalSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Spawn!"));

		AActor* SpawnedPortal = GetWorld()->SpawnActor<AClearPortal>(
			PortalClass,
			PortalSpawnPoint->GetActorLocation(),
			FRotator::ZeroRotator
		);

		if (SpawnedPortal)
		{
			UE_LOG(LogTemp, Warning, TEXT("Portal Spawned Successfully at Location: %s"), *PortalSpawnPoint->GetActorLocation().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Portal Spawn Failed!"));
		}
	}
}

void AFPSGameMode::TravelToLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	APlayerController* PlayerController = Cast<APlayerController>(World->GetFirstPlayerController());
	if (!PlayerController) return;

	AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
	if (!FPSPlayerController) return;
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(FPSPlayerController->GetPawn());
	if (!PlayerCharacter) return;
	
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (FPSGameInstance && PlayerCharacter)
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(World->StreamingLevelsPrefix);
		FPSGameInstance->SetPreviousLevel(CurrentLevel);
		FPSGameInstance->SaveMouseSensitivity();
		FPSGameInstance->SaveWeaponStats(PlayerCharacter);
		UE_LOG(LogTemp, Warning, TEXT("Saved Previous Level: %s"), *CurrentLevel);
	}
	SavePlayerLocation();
	World->ServerTravel(LevelName.ToString(), true);
}

void AFPSGameMode::ReturnToPreviousLevel()
{
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (FPSGameInstance)
	{
		FString PreviousLevel = FPSGameInstance->GetPreviousLevel();
		if (!PreviousLevel.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Returning to Level: %s"), *PreviousLevel);
			GetWorld()->ServerTravel(PreviousLevel, true);
			bIsInTrapLevel = true;
			FPSGameInstance->bIsInTrapLevel = bIsInTrapLevel;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PreviousLevel is Empthy"));
		}
	}
}

void AFPSGameMode::SpawnTrapPortals()
{
	UFPSGameInstance* FPSGameInstnace = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!FPSGameInstnace) return;

	AActor* TrapPortalSpawnPoint = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapPortalPoint::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor && Actor->ActorHasTag(FName("TrapPortalPoint")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Actor: %s"), *Actor->GetFName().ToString());
			TrapPortalSpawnPoint = Actor;
			break;
		}
	}

	if (TrapPortalSpawnPoint)
	{
		FActorSpawnParameters SpawnParams;
		ATrapPortal* SpawnedPortal = GetWorld()->SpawnActor<ATrapPortal>(
			TrapPortalClass,
			TrapPortalSpawnPoint->GetActorLocation(),
			FRotator::ZeroRotator,
			SpawnParams
			);

		if (SpawnedPortal)
		{
			if (ATrapPortalPoint* TrapPortalPoint = Cast<ATrapPortalPoint>(TrapPortalSpawnPoint))
			{
				if (TrapPortalPoint->PortalAction == ETrapPortalAction::Entry)
				{
					SpawnedPortal->PortalType = ETrapPortalTypes::TravelToTrap;
				}
				else if (TrapPortalPoint->PortalAction == ETrapPortalAction::Exit)
				{
					SpawnedPortal->PortalType = ETrapPortalTypes::ReturnToStage;
				}
			}
		}
	}
}

void AFPSGameMode::SavePlayerLocation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (!FPSPlayerController || !FPSPlayerController->GetPawn()) return;

		UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
		
		if (FPSGameInstance)
		{
			FPSGameInstance->StoredPlayerLocation = FPSPlayerController->GetPawn()->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Saved Player Location: %s"), *FPSGameInstance->StoredPlayerLocation.ToString());
		}	
	}	
}

void AFPSGameMode::RestorePlayerLocation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		AMyPlayerController* FPSPlayerController = Cast<AMyPlayerController>(PlayerController);
		if (!FPSPlayerController || !FPSPlayerController->GetPawn()) return;

		UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
		
		if (FPSGameInstance)
		{
			FVector SavedLocation = FPSGameInstance->StoredPlayerLocation;
			FPSPlayerController->GetPawn()->SetActorLocation(SavedLocation, false, nullptr, ETeleportType::TeleportPhysics);
			UE_LOG(LogTemp, Warning, TEXT("Restored Player Location: %s"), *SavedLocation.ToString());
		}	
	}
}

void AFPSGameMode::LoadCardDataFromDataTable()
{
	if (!CardDataTable) return;

	static const FString ContextString(TEXT("PassiveCardContext"));
	TArray<FCardDataTable*> CardRows;
	CardDataTable->GetAllRows<FCardDataTable>(ContextString, CardRows);

	for (FCardDataTable* Row : CardRows)
	{
		UCardData* NewCard = NewObject<UCardData>();
		NewCard->CardName = Row->CardName;
		NewCard->CardDescription = Row->CardDescription;
		NewCard->Rarity = Row->Rarity;
		NewCard->CardEffect = Row->CardEffect;

		AllCardPool.Add(NewCard);
	}

	UE_LOG(LogTemp, Log, TEXT("Loaded %d cards into AllCardPool"), AllCardPool.Num());
}

void AFPSGameMode::ShowCardSelectionUI()
{
	SetGameState(EGameState::CardSelection);
	
	if (CardSelectionWidgetClass)
	{
		UIngameSelectWidget* CardSelectionWidget = CreateWidget<UIngameSelectWidget>(GetWorld(), CardSelectionWidgetClass);
		if (CardSelectionWidget)
		{
			CardSelectionWidget->AddToViewport();
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(CardSelectionWidget->TakeWidget());
			PlayerController->SetInputMode(InputMode);
		}
	}
}

void AFPSGameMode::ContinueGameAfterCardSelection()
{
	SetGameState(EGameState::Playing);
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AFPSGameMode::SetGameState(EGameState NewState)
{
	if (CurrentGameState != NewState)
	{
		CurrentGameState = NewState;
	}
}

UCardData* AFPSGameMode::GetRandomCard()
{
	if (AllCardPool.Num() == 0)
	{
		return nullptr;
	}

	TArray<UCardData*> CommonCards;
	TArray<UCardData*> RareCards;
	TArray<UCardData*> LegendaryCards;

	for (UCardData* Card : AllCardPool)
	{
		switch (Card->Rarity)
		{
		case ECardRarity::Common:
			CommonCards.Add(Card);
			break;
		case ECardRarity::Rare:
			RareCards.Add(Card);
			break;
		case ECardRarity::Legendary:
			LegendaryCards.Add(Card);
			break;
		}
	}

	float Roll = FMath::FRand();

	if (Roll < 0.6f && CommonCards.Num() > 0)
	{
		return CommonCards[FMath::RandRange(0, CommonCards.Num() - 1)];
	}
	else if (Roll < 0.95f && RareCards.Num() > 0)
	{
		return RareCards[FMath::RandRange(0, RareCards.Num() - 1)];
	}
	else if (LegendaryCards.Num() > 0)
	{
		return LegendaryCards[FMath::RandRange(0, LegendaryCards.Num() - 1)];
	}

	return nullptr;
}

TArray<UCardData*> AFPSGameMode::GetRandomCards(int32 CardCount)
{
	TArray<UCardData*> SelectedCards;

	for (int32 i = 0; i < CardCount; i++)
	{
		UCardData* RandomCard = GetRandomCard();
		if (RandomCard)
		{
			SelectedCards.Add(RandomCard);
		}
	}

	return SelectedCards;
}

void AFPSGameMode::InitializeDropManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	if (DropManager)
	{

		return;
	}

	if (DropManagerClass)
	{
		DropManager = World->SpawnActor<ADropManager>(DropManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}
}

void AFPSGameMode::PlayMainHudShow()
{
	if (PlayerMainHudClass)
	{
		UIngameMainWidget* PlayerMainWidget = CreateWidget<UIngameMainWidget>(GetWorld(), PlayerMainHudClass);
		if (PlayerMainWidget)
		{
			PlayerMainWidget->AddToViewport();
		}
	}
}

ADropManager* AFPSGameMode::GetDropManager()
{
	return DropManager;
}

void AFPSGameMode::ClearAllEnemies()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, ABaseEnemy::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
		}
	}
}

void AFPSGameMode::ClearAllBullets()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> Bullets;
	UGameplayStatics::GetAllActorsOfClass(World, ABulletBase::StaticClass(), Bullets);

	for (AActor* Bullet : Bullets)
	{
		if (Bullet)
		{
			Bullet->Destroy();
		}
	}

}

void AFPSGameMode::EndGame(bool bPlayWin)
{
	if (bPlayWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));
		
		FTimerHandle EndTimerHandle;
		GetWorldTimerManager().SetTimer(
			EndTimerHandle,
			this,
			&AFPSGameMode::ReturnToMainMenu,
			5.0f,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
		
		FTimerHandle EndTimerHandle;
		GetWorldTimerManager().SetTimer(
			EndTimerHandle,
			this,
			&AFPSGameMode::ReturnToMainMenu,
			5.0f,
			false
		);
	}
}

void AFPSGameMode::ReturnToMainMenu()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GameInstance))
		{
			if (FPSGameInstance)
			{
				FPSGameInstance->GotoMainMenu();
			}
		}
	}	
}

ABulletPool* AFPSGameMode::GetBulletPool()
{
	return BulletPoolInstance;
}

