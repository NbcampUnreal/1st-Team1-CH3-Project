// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "PierceBullet.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API APierceBullet : public ABulletBase
{
	GENERATED_BODY()
	
	APierceBullet();
	
	virtual void BeginPlay() override;
	
	virtual void Fire(FVector StartLocation, FVector Direction, float GunDamage,float GunSpeed) override;
	
	virtual void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	//총기꼬리 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* BulletTail;

private:
	int PierceCount = 0;

	
};
