// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MinimapManager.generated.h"

/**
 * 
 */

USTRUCT()
struct FMinimapTargetLocation
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, Category = "Minimap")
	AActor* TargetActor;

	UPROPERTY(VisibleInstanceOnly, Category = "Minimap")
	FVector Location;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetUpdated, const TArray<FMinimapTargetLocation>&, UpdateLocations);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetRemoved, AActor*, RemovedTarget);

UCLASS()
class GUNFIREPARAGON_API UMinimapManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize();
	void RegisterTarget(AActor* Target);
	void UnRegisterTarget(AActor* Target);
	void RecvLocationToBroadCast();

public:
	UPROPERTY(BlueprintAssignable, Category = "Minimap")
	FOnTargetUpdated OnAllLocationsUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Minimap")
	FOnTargetRemoved OnTargetRemoved;

private:
	TMap<AActor*,FVector> TargetLocations;
	FTimerHandle UpdateTimer;

	void UpdateTargetsLocation();
};
