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
	AmmoType = EAmmoType::Normal;
	WeaponType = EWeaponType::Shotgun;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Test/TestGun/simple-old-shotgun/source/SKM_shotgun.SKM_shotgun'"));
	if (RifleMesh.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(RifleMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/buckyTap.buckyTap"));
    			if (SoundAsset.Succeeded())
    			{
    				FireSound = SoundAsset.Object;
    			}

	
	SwitchGunSound(WeaponType);
}

void AGun_Shotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AGun_Shotgun::Fire()
{
	if (!CanFire()) 
	{
		return;
	}

	//  발사 후 탄약 감소
	CurrentAmmo-=Pellets;

	if (GunMesh && GunMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = GunMesh->GetSocketLocation(TEXT("Muzzle"));
		
		if (MuzzleFlashEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), 
				MuzzleFlashEffect,  // 머즐 플래시 이펙트
				MuzzleSpot,          // 위치 (총구)
				GunMesh->GetSocketRotation(TEXT("Muzzle")) // 총구 회전 방향
			);
		}
	}

	if (FireSound.IsValid())
	{
		
		UGameplayStatics::PlaySoundAtLocation(this, FireSound.Get(), GetActorLocation());
	}
	//  샷건 탄환 여러 개 발사
	if ( CurrentAmmo<Pellets)
	{
		for (int i = 0; i < CurrentAmmo; i++)
		{
			// 총알 퍼짐 방향 설정
			FVector forwardDirection = GetAimDirection();
			forwardDirection = SpreadDirection(forwardDirection);
	
			//  총알을 풀에서 가져오기
			ABulletBase* Bullet = BulletPool->GetPooledBullet(EAmmoType::Normal);
			if (Bullet)
			{
				Bullet->Fire(MuzzleSpot, forwardDirection, Damage,GunSpeed);
			}
		}
	}
	else
	{
		for (int i = 0; i < Pellets; i++)
		{
        UE_LOG(LogTemp, Error, TEXT("BulletCount"));
        //UE_LOG(LogTemp, Error, TEXT("한번에나가는 탄약: %i",Pellts));
			// 총알 퍼짐 방향 설정
			FVector forwardDirection = GetAimDirection();
			forwardDirection = SpreadDirection(forwardDirection);
	
			//  총알을 풀에서 가져오기
			ABulletBase* Bullet = BulletPool->GetPooledBullet(EAmmoType::Normal);
			if (Bullet)
			{
				Bullet->Fire(MuzzleSpot, forwardDirection, Damage,GunSpeed);
			}
		}
	
	}
	bCanFire = false;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AGun_Shotgun::SetIsFire, GunDelay-0.01f, false);
}