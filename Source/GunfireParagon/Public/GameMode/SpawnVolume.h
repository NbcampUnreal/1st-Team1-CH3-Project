// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIEnemySpawnRaw.h"
#include "AI/BaseEnemy.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GUNFIREPARAGON_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

	FVector GetRandomPointInVolume() const;
	FVector GetSafeSpawnPoint() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	UDataTable* AIEnemyTalbe;
	
};
