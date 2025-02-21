// Fill out your copyright notice in the Description page of Project Settings.


#include "PierceBullet.h"
#include "BulletPool.h"
#include "Kismet/GameplayStatics.h"

APierceBullet::APierceBullet()
{
	PierceCount = 3;
	BulletDamage = 50.0f;
	AmmoType = EAmmoType::Pierce;
}

void APierceBullet::BeginPlay()
{
	Super::BeginPlay();
}

void APierceBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage)
{
	Super::Fire(StartLocation, Direction, GunDamage);
}

void APierceBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBulletOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!OtherActor || OtherActor == this) 
	{
		UE_LOG(LogTemp, Warning, TEXT("자신충돌했으나 무시함"));
		return;
	}
	if (OtherActor->ActorHasTag("Bullet"))
	{
		UE_LOG(LogTemp, Warning, TEXT("총알이 다른총알과 충돌했으나 무시됨: %s"), *OtherActor->GetName());
		return;
	}
	if (OtherActor->ActorHasTag("Gun"))
	{
		UE_LOG(LogTemp, Warning, TEXT("총알이 총(Gun)과 충돌했으나 무시됨: %s"), *OtherActor->GetName());
		return;
	}
	
	float FinalDamage = BulletDamage;
	
	if (OtherActor->ActorHasTag("Enemy"))
	{
		PierceCount--;
		// 맞은 부위가 "head"인지 확인 (Skeleton Bone Name 사용)
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			FinalDamage *= 2.0f; // 헤드샷 데미지 2배
			UE_LOG(LogTemp, Warning, TEXT("헤드샷! 데미지: %f"), FinalDamage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("일반 공격! 데미지: %f"), FinalDamage);
		}
		//  ApplyPointDamage 사용 (맞은 위치 포함)
		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("총알이 Enemy를 맞춤: %s"), *OtherActor->GetName());
		
		if (PierceCount<=0)
		{
			if (BulletPool)
			{
				BulletPool->ReturnBullet(this, AmmoType);
				UE_LOG(LogTemp, Warning, TEXT("%s 총알이 %s 맞춤"),*GetName() ,*OtherActor->GetName());
				UE_LOG(LogTemp, Warning, TEXT("Normal Bullet__풀링으로 반환됨"));
			}
			return;
		}
	}


	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
		UE_LOG(LogTemp, Warning, TEXT("%s 총알이 %s 맞춤"),*GetName() ,*OtherActor->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Normal Bullet__풀링으로 반환됨"));
	}

	
}
