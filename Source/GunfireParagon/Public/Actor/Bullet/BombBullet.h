// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletBase.h"
#include "BombBullet.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIREPARAGON_API ABombBullet : public ABulletBase
{
	GENERATED_BODY()


	ABombBullet();

	
	virtual void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	

private:
	void ApplyExplosionDamage();
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Projectile")
	int32 ExplosionRadius = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Sounds")
	TSoftObjectPtr<USoundBase> ExplosionSound;

	virtual void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
