// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/Gun_Shotgun.h"
#include "Actor/Bullet/BulletBase.h"  
#include "Actor/BulletPool.h" 
#include "Kismet/GameplayStatics.h"


AGun_Shotgun::AGun_Shotgun()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bIsAutoFire = false;
	Pellets = 6; //  샷건 탄환 개수 설정 (한 번 발사 시 발사될 총알 개수)
	MaxAmmo =100;
	AmmoType = EAmmoType::Normal;
}

void AGun_Shotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AGun_Shotgun::Fire()
{
	if (!CanFire()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("샷건 발사 불가! 탄약 부족 또는 발사 타이머 동작 중"));
		return;
	}

	//  발사 후 탄약 감소
	CurrentAmmo-=Pellets;

	if (GunMesh && GunMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = GunMesh->GetSocketLocation(TEXT("Muzzle"));
	}

	//  샷건 탄환 여러 개 발사
	for (int i = 0; i < Pellets; i++)
	{
		// 총알 퍼짐 방향 설정
		FVector forwardDirection = GetAimDirection();
		forwardDirection = SpreadDirection(forwardDirection);
	
		//  총알을 풀에서 가져오기
		ABulletBase* Bullet = BulletPool->GetPooledBullet(EAmmoType::Normal);
		if (Bullet)
		{
			Bullet->Fire(MuzzleSpot, forwardDirection, Damage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("사용 가능한 총알이 없습니다!"));
		}
	}

	//  발사 후 일정 시간 동안 추가 발사 금지 (샷건은 단발 무기)
	bCanFire = false;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AGun_Shotgun::SetIsFire, GunDelay-0.01f, false);
}