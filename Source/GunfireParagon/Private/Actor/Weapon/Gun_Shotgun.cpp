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

	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		
		if (MuzzleFlashEffect)
		{
			FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(TEXT("Muzzle")) - WeaponMesh->GetComponentRotation();
			MuzzleRotation.Yaw -= 90.0f; // Y축 정렬 보정
			UNiagaraComponent* MuzzleEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				MuzzleFlashEffect,           // 나이아가라 시스템
				WeaponMesh,                  // 부모: 무기 메쉬
				TEXT("Muzzle"),              // 소켓 이름
				FVector::ZeroVector,         // 상대 위치 (소켓 기준)
				MuzzleRotation,              // 초기 회전값 (부모 회전 제거)
				EAttachLocation::SnapToTarget, // 부모 위치 & 소켓에 정확히 부착
				true                         // 자동 파괴
			);

			if (MuzzleEffectComp)
			{
				MuzzleEffectComp->SetAutoDestroy(true); 
				MuzzleEffectComp->SetVariableFloat(FName("User.Lifetime"), 0.2f);
				FTimerHandle EffectDestroyTimer;
				GetWorld()->GetTimerManager().SetTimer(EffectDestroyTimer, [MuzzleEffectComp]()
				{
					if (MuzzleEffectComp)
					{
						MuzzleEffectComp->DestroyComponent();
					}
				}, 0.2f, false);
			}
		}


		
		FVector AimDirection = GetAimDirection();

		
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
	
				FVector forwardDirection = SpreadDirection(AimDirection);
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
				
				FVector forwardDirection = SpreadDirection(AimDirection);
				//  총알을 풀에서 가져오기
				ABulletBase* Bullet = BulletPool->GetPooledBullet(EAmmoType::Normal);
				if (Bullet)
				{
					Bullet->Fire(MuzzleSpot, forwardDirection, Damage,GunSpeed);
				}
				DrawDebugLine(GetWorld(), MuzzleSpot, MuzzleSpot + (forwardDirection * 1000.0f), FColor::Red, false, 1.0f, 0, 1.0f);
			}
	
		}
		bCanFire = false;
		GetWorldTimerManager().SetTimer(FireTimer, this, &AGun_Shotgun::SetIsFire, GunDelay-0.01f, false);
	}
}