// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InGameMinimapDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API UInGameMinimapDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	TArray<FName> LevelNames;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	TArray<UTexture2D*> Textures;
};
