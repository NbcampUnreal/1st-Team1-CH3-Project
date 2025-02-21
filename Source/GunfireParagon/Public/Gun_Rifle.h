// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGunBase.h"
#include "Gun_Rifle.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API AGun_Rifle : public ACGunBase
{
	GENERATED_BODY()

public:
	
	AGun_Rifle();

	virtual void BeginPlay() override;

	virtual void Fire() override;

	
	
	
};
