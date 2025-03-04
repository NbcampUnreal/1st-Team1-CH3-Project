// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponTextureMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	TSubclassOf<class ACGunBase> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	UTexture2D* WeaponTexture;
};

UCLASS()
class GUNFIREPARAGON_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon UI")
	TArray<FWeaponTextureMapping> WeaponTextures;
};
