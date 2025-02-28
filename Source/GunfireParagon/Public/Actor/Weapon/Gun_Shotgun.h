// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CGunBase.h"
#include "Gun_Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API AGun_Shotgun : public ACGunBase
{
	GENERATED_BODY()


public:
	AGun_Shotgun();

protected:
	virtual void BeginPlay() override;
	virtual void Fire() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunStatus")
	int32 Pellets = 6; // 샷건 발사 시 총알 개수
	
};
