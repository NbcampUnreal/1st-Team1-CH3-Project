#include "Actor/Weapon/CGunBase.h"
#include "Actor/Bullet/BulletBase.h"  
#include "Actor/BulletPool.h" 
#include "Kismet/GameplayStatics.h"


ACGunBase::ACGunBase()
{
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);
}







void ACGunBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MaxAmmo;

    UE_LOG(LogTemp, Warning, TEXT("총기 장착됨: %s, 초기 탄약: %d"), *GetName(), CurrentAmmo);

    if (WeaponMesh)
    {
        if (WeaponMesh->GetSkeletalMeshAsset())
        {
            UE_LOG(LogTemp, Warning, TEXT("WeaponMesh에 SkeletalMesh가 정상적으로 설정됨: %s"),
                *WeaponMesh->SkeletalMesh->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WeaponMesh에 SkeletalMesh가 설정되지 않음!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponMesh가 초기화되지 않음!"));
    }
    //  BulletPool을 찾기 (월드에서 검색)
    BulletPool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));

    if (!BulletPool)
    {
        UE_LOG(LogTemp, Warning, TEXT("BulletPool이 월드에 없음. 새로 생성합니다."));

        BulletPool = GetWorld()->SpawnActor<ABulletPool>(ABulletPool::StaticClass());

        if (BulletPool)
        {
            UE_LOG(LogTemp, Warning, TEXT("BulletPool이 새로 생성되었습니다."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BulletPool 생성 실패! 총알 풀링이 동작하지 않을 수 있습니다."));
        }
    }


}


void ACGunBase::Fire()
{
    UE_LOG(LogTemp, Warning, TEXT("CGunBase::Fire() 실행됨 - 현재 탄약: %d"), CurrentAmmo);

    if (!CanFire())
    {
        UE_LOG(LogTemp, Warning, TEXT("발사 불가! 탄약 부족 또는 발사 타이머 동작 중"));
        return;
    }

    bCanFire = false; //발사 후 즉시 다음 발사 방지
    GetWorldTimerManager().SetTimer(FireTimer, this, &ACGunBase::SetIsFire, GunDelay, false); //딜레이 적용

    UE_LOG(LogTemp, Warning, TEXT("Fire() 호출됨 - 현재 탄약: %d"), CurrentAmmo);

    CurrentAmmo--;

    if (IsAmmoEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("탄약 부족! 자동 재장전 실행"));
        Reload();
        return;
    }

    FVector FireDirection;
    if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
    {
        MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
        FireDirection = WeaponMesh->GetSocketRotation(TEXT("Muzzle")).Vector();
        FireDirection.Normalize(); //방향 벡터 정규화
        UE_LOG(LogTemp, Warning, TEXT("Muzzle 위치: %s, 발사 방향: %s"),
            *MuzzleSpot.ToString(), *FireDirection.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Muzzle 소켓이 존재하지 않음! 기본 전방 방향 사용"));
        FireDirection = GetActorForwardVector();
    }

    if (!BulletPool)
    {
        UE_LOG(LogTemp, Error, TEXT("BulletPool이 없음! 총알을 생성할 수 없음"));
        return;
    }

    ABulletBase* Bullet = BulletPool->GetPooledBullet(AmmoType);

    if (Bullet)
    {
        Bullet->Fire(MuzzleSpot, FireDirection, Damage);
        UE_LOG(LogTemp, Warning, TEXT("총알 발사 완료! 탄환 타입: %d"), (int32)AmmoType);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("사용 가능한 총알이 없음!"));
    }
}




bool ACGunBase::CanFire() const
{
	//bCanFire가 false면 발사 불가능 (GunDelay 동안 발사 막기)
	if (!bCanFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("발사 대기 중!"));
		return false;
	}

	//탄약이 없으면 발사 불가능
	return !IsAmmoEmpty();
}

void ACGunBase::StopFire()
{
	if (bIsAutoFire)
	{
		GetWorldTimerManager().ClearTimer(AutoFireTimer);
		UE_LOG(LogTemp, Warning, TEXT("자동 발사 중지"));
	}
}

void ACGunBase::SetIsFire()
{
	bCanFire = true;
}

bool ACGunBase::IsAmmoEmpty() const
{
	return CurrentAmmo <= 0;
}

void ACGunBase::Reload()
{
	//탄창이 이미 가득 차 있다면 재장전 필요 없음
	if (CurrentAmmo == MaxAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("탄창이 이미 가득 찼습니다!"));
		return;
	}

	// 탄창을 가득 채움
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogTemp, Warning, TEXT("재장전 완료! 현재 탄약: %d"), CurrentAmmo);
}

FVector ACGunBase::SpreadDirection(const FVector OriginDirection) const
{
	float RandomYaw = FMath::RandRange(-GunSpread, GunSpread);
	float RandomPitch = FMath::RandRange(-GunSpread, GunSpread);

	FRotator SpreadRotation = FRotator(RandomPitch, RandomYaw, 0.0f);
	return SpreadRotation.RotateVector(OriginDirection);
}