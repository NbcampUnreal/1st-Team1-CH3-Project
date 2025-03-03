#include "Actor/Weapon/CGunBase.h"
#include "Actor/Bullet/BulletBase.h"  
#include "Actor/BulletPool.h" 
#include "Player/MyPlayerController.h" 
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


ACGunBase::ACGunBase()
{
	PrimaryActorTick.bCanEverTick = false;

	
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	RootComponent = SceneComponent;
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(RootComponent);
	
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);

	DisableWeaponShadows();

	if (!MuzzleFlashEffect)
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MuzzleEffectFinder(TEXT("/Game/VFX/TakeGame/NS_MuzzleFlash.NS_MuzzleFlash"));
		if (MuzzleEffectFinder.Succeeded())
		{
			MuzzleFlashEffect = MuzzleEffectFinder.Object;
			UE_LOG(LogTemp, Warning, TEXT("MuzzleFlashEffect 로드 성공: %s"), *MuzzleFlashEffect->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MuzzleFlashEffect 로드 실패!"));
		}
	}
}







void ACGunBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MaxAmmo;

    UE_LOG(LogTemp, Warning, TEXT("총기 장착됨: %s, 초기 탄약: %d"), *GetName(), CurrentAmmo);

	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		UE_LOG(LogTemp, Warning, TEXT("총구 위치 설정됨: %s"), *MuzzleSpot.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponMesh 'Muzzle' 소켓이 존재하지 않습니다. 기본 위치 사용."));
	}

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
    }
	
	

}


void ACGunBase::Fire()
{
	if (!MuzzleFlashEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ MuzzleFlashEffect가 nullptr! 블루프린트에서 확인 필요!"));
	}
	if (!CanFire())
		return;
	
    UE_LOG(LogTemp, Warning, TEXT("CGunBase::Fire() 실행됨 - 현재 탄약: %d"), CurrentAmmo);

	
	bCanFire = false; //발사 후 즉시 다음 발사 방지
	GetWorldTimerManager().SetTimer(FireTimer, this, &ACGunBase::SetIsFire, GunDelay, false); //딜레이 적용
	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		UE_LOG(LogTemp, Warning, TEXT("총구 위치 설정됨: %s"), *MuzzleSpot.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponMesh 'Muzzle' 소켓이 존재하지 않습니다. 기본 위치 사용."));
	}
	
	if (MuzzleFlashEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("발사이펙트!"));


		FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(TEXT("Muzzle")) - WeaponMesh->GetComponentRotation();
		MuzzleRotation.Yaw -= 90.0f; // Y축 정렬 보정
		
		UE_LOG(LogTemp, Warning, TEXT("MuzzleRotation (Adjusted): Pitch=%f, Yaw=%f, Roll=%f"),
			MuzzleRotation.Pitch, MuzzleRotation.Yaw, MuzzleRotation.Roll);

		// 🔹 나이아가라 이펙트를 총구에 부착
		UNiagaraComponent* MuzzleEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashEffect,           // 나이아가라 시스템
			WeaponMesh,                  // 부모: 무기 메쉬
			TEXT("Muzzle"),              // 소켓 이름
			FVector::ZeroVector,         // 상대 위치 (소켓 기준)
			MuzzleRotation,              // 초기 회전값 (부모 회전 제거)
			EAttachLocation::SnapToTarget, // 부모 위치 & 소켓에 정확히 부착
			true                         // 자동 파괴
		);

		// FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(TEXT("Muzzle"));
		//
		// MuzzleRotation.Yaw -= 90.0f; // Yaw 보정
		// UNiagaraComponent* MuzzleEffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		// 	GetWorld(),
		// 	MuzzleFlashEffect,
		// 	MuzzleSpot,
		// 	MuzzleRotation, // 보정된 회전값 사용
		// 	FVector(1, 1, 1), // 크기 조정
		// 	true // 자동 파괴
		// );

		
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

	
	FVector forwardDirection = GetAimDirection();

	//FVector forwardDirection = GetActorForwardVector();
	
	//  총알을 풀에서 가져오기
	ABulletBase* Bullet = BulletPool->GetPooledBullet(AmmoType);
	if (Bullet)
	{
		Bullet->Fire(MuzzleSpot, forwardDirection, Damage);
		UE_LOG(LogTemp, Warning, TEXT("총알 발사! 속도: %f"), Bullet->ProjectileMovement->InitialSpeed);

		CurrentAmmo--;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("사용 가능한 총알이 없습니다!"));
	}
	
}




bool ACGunBase::CanFire() 
{
	//bCanFire가 false면 발사 불가능 (GunDelay 동안 발사 막기)
	if (!bCanFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("GunDelay중!"));
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

bool ACGunBase::IsAmmoEmpty() 
{
	//테스트용 리로드
	Reload();
	UE_LOG(LogTemp, Warning, TEXT("탄약 부족! 자동 재장전 실행"));
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

FVector ACGunBase::GetAimDirection() const
{

	FVector MuzzleLocation = FVector::ZeroVector;
	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		UE_LOG(LogTemp, Warning, TEXT("총구 위치 설정됨: %s"), *MuzzleSpot.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponMesh 'Muzzle' 소켓이 존재하지 않습니다. 기본 위치 사용."));
	}
	
	//싱글플레이어라 0번을 가져옴
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
		return GetActorForwardVector(); // 기본값: 총이 향하는 방향

	//  2. 플레이어의 컨트롤러 가져오기
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
		return GetActorForwardVector();


	
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.0f); // 먼 거리까지 라인 트레이스

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(PlayerCharacter);
	QueryParams.AddIgnoredActor(GetOwner());

	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		FVector AimDirection = (HitResult.ImpactPoint - MuzzleSpot).GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("MuzzleSpot: %s, ImpactPoint: %s"), *MuzzleSpot.ToString(), *HitResult.ImpactPoint.ToString());
		if (FVector::DistSquared(HitResult.ImpactPoint, MuzzleSpot) < 100.0f) 
		{
			UE_LOG(LogTemp, Warning, TEXT("AimDirection 너무 가까움! 기본 방향 사용"));
			return CameraRotation.Vector();  
		}
		
		UE_LOG(LogTemp, Warning, TEXT("총알방향: %s"), *AimDirection.ToString());
		UE_LOG(LogTemp, Warning, TEXT("화면 정중앙으로 발사"));

		return AimDirection;
		
	}

	return CameraRotation.Vector();
}



FVector ACGunBase::SpreadDirection(const FVector OriginDirection) const
{
	float RandomYaw = FMath::RandRange(-GunSpread, GunSpread);
	float RandomPitch = FMath::RandRange(-GunSpread, GunSpread);

	FRotator SpreadRotation = FRotator(RandomPitch, RandomYaw, 0.0f);
	return SpreadRotation.RotateVector(OriginDirection);
}


void ACGunBase::DisableWeaponShadows()
{
	if (WeaponMesh)
	{
		WeaponMesh->SetCastShadow(false);
		WeaponMesh->bCastDynamicShadow = false;
		WeaponMesh->bAffectDistanceFieldLighting = false;
	}
}

