// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InGameStateDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FStateMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FindName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* TextureFile;
};

UCLASS(Blueprintable)
class GUNFIREPARAGON_API UInGameStateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture")
	TArray<FStateMapping> StateMappings;
	
};
