// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Bullet/PierceBullet.h"
#include "Actor/BulletPool.h"
#include "Kismet/GameplayStatics.h"

APierceBullet::APierceBullet()
{
	PierceCount = 3;
	BulletDamage = 50.0f;
	AmmoType = EAmmoType::Pierce;

	if (!BulletTail)
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BulletTailFinder(TEXT("/Game/VFX/TakeGame/NS_Projectile_01.NS_Projectile_01"));
		if (BulletTailFinder.Succeeded())
		{
			BulletTail = BulletTailFinder.Object;
		}
	}
}

void APierceBullet::BeginPlay()
{
	Super::BeginPlay();
}

void APierceBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage,float GunSpeed)
{
	Super::Fire(StartLocation, Direction, GunDamage,GunSpeed);
	if (BulletTail)
	{
		
		FVector BulletLocation = GetActorLocation();
		FRotator BulletRotation = GetActorRotation();
		USceneComponent* RootComp = GetRootComponent();
		if (!RootComp)
		{
			return;
		}
		UNiagaraComponent* BulletTailComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BulletTail,         
			RootComp,                  
			NAME_None,           
			FVector::ZeroVector,   // 상대 위치 (총알 중심)
			BulletRotation,        // 총알의 회전 값을 사용
			EAttachLocation::SnapToTarget, // 정확히 부착
			true                   // 자동 파괴
		);

		if (BulletTailComp)
		{
			BulletTailComp->SetAutoDestroy(true);
			BulletTailComp->SetVariableFloat(FName("User.Lifetime"), 0.2f);

			FTimerHandle EffectDestroyTimer;
			GetWorld()->GetTimerManager().SetTimer(EffectDestroyTimer, [BulletTailComp]()
			{
				if (BulletTailComp)
				{
					BulletTailComp->DestroyComponent();
				}
			}, 2.0f, false);
		}


		
	}
}

void APierceBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBulletOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!OtherActor || OtherActor == this) 
	{
		return;
	}
	if (OtherActor->ActorHasTag("Bullet"))
	{
		return;
	}
	if (OtherActor->ActorHasTag("Gun"))
	{
		return;
	}
	
	float FinalDamage = BulletDamage;
	
	if (OtherActor->ActorHasTag("Enemy"))
	{
		bool IsHead = false;
		PierceCount--;
		// 맞은 부위가 "head"인지 확인 (Skeleton Bone Name 사용)
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			IsHead = true;
			FinalDamage *= 2.0f; // 헤드샷 데미지 2배
		}
		else
		{
		}
		//  ApplyPointDamage 사용 (맞은 위치 포함)
		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());
		OnHitMarker.Broadcast(IsHead);
		if (PierceCount<=0)
		{
			if (BulletPool)
			{
				SpawnBulletDecal(SweepResult);
				BulletPool->ReturnBullet(this, AmmoType);
			}
			return;
		}
	}


	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
	}

	
}
