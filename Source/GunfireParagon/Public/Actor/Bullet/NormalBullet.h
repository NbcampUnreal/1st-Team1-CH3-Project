// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "NormalBullet.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API ANormalBullet : public ABulletBase
{
	GENERATED_BODY()

protected:
	ANormalBullet();
	
	virtual void BeginPlay() override;
	
	virtual void Fire(FVector StartLocation, FVector Direction, float GunDamage) override;
	
	virtual void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
