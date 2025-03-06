#include "Actor/Weapon/CGunBase.h"
#include "Actor/Bullet/BulletBase.h"  
#include "Actor/BulletPool.h" 
#include "Player/MyPlayerController.h" 
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/FPSGameMode.h"
#include "FPSDataTables.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


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
		}
	}

	SwitchGunSound(WeaponType);
	
}




void ACGunBase::BeginPlay()
{
    Super::BeginPlay();

	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	}

	AFPSGameMode* GameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		BulletPool = GameMode->GetBulletPool();
	}
	
	AFPSDataTables* DataTables = Cast<AFPSDataTables>(UGameplayStatics::GetActorOfClass(GetWorld(), AFPSDataTables::StaticClass()));

	if (!DataTables)
	{
		DataTables = GetWorld()->SpawnActor<AFPSDataTables>();
	}
	if (DataTables)
	{
		FWeaponData* WeaponData = DataTables->GetWeaponDataByKey(WeaponDataKey);
		if (WeaponData)
		{
			Damage = WeaponData->AttackPower;
			GunDelay = WeaponData->GunDelay;
			MaxAmmo = WeaponData->MaxAmmo;
			GunSpeed = WeaponData->AttackSpeed;
			CurrentAmmo = MaxAmmo;
			DropEffect = Cast<UNiagaraSystem>(WeaponData->DropEffectPath.TryLoad());
			

			if (WeaponData->DropEffectPath.IsValid())
			{
				FString AssetPath = WeaponData->DropEffectPath.ToString();

				UObject* LoadedObject = StaticLoadObject(UNiagaraSystem::StaticClass(), nullptr, *AssetPath);
				if (LoadedObject)
				{
					DropEffect = Cast<UNiagaraSystem>(LoadedObject);
				}
			}
		}
	}
	if (DropEffect)
	{
		DropEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DropEffect,
			WeaponMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
	SetIsDrop(true);
}

void ACGunBase::SetIsDrop(bool isDrop)
{
	bISDrop = isDrop;
	if (bISDrop)
	{
		if (DropEffectComp)
		{
			DropEffectComp->Activate(isDrop);
		}
	}
	else
	{
		if (DropEffectComp)
		{
			DropEffectComp->Deactivate();
			DropEffectComp->SetVisibility(false);
		}
	}
	
}

void ACGunBase::Fire()
{
	
	if (!CanFire())
		return;
	

	
	bCanFire = false; //발사 후 즉시 다음 발사 방지
	
	GetWorldTimerManager().SetTimer(FireTimer, this, &ACGunBase::SetIsFire, GunDelay, false); //딜레이 적용

	if (FireSound.IsValid())
	{
		
		UGameplayStatics::PlaySoundAtLocation(this, FireSound.Get(), GetActorLocation());
	}
	
	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	}
	else
	{
	}
	
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

	
	FVector forwardDirection = GetAimDirection();
	//  총알을 풀에서 가져오기
	ABulletBase* Bullet = BulletPool->GetPooledBullet(AmmoType);
	if (Bullet)
	{
		Bullet->Fire(MuzzleSpot, forwardDirection, Damage,GunSpeed);

		CurrentAmmo--;
	}
	
}




bool ACGunBase::CanFire() 
{
	//bCanFire가 false면 발사 불가능 (GunDelay 동안 발사 막기)
	if (!bCanFire)
	{
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
	return CurrentAmmo <= 0;
}

void ACGunBase::SwitchGunSound(EWeaponType WeaponClass)
{
	
	switch (WeaponClass)
	{
	case EWeaponType::Rifle:
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/bulldogTap.bulldogTap"));
			if (SoundAsset.Succeeded())
			{
				FireSound = SoundAsset.Object;
			}
		}
		break;

	case EWeaponType::Shotgun:
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/buckyTap.buckyTap"));
			if (SoundAsset.Succeeded())
			{
				FireSound = SoundAsset.Object;
			}
		}
		break;
	case EWeaponType::Sniper:
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/operatorTap.operatorTap"));
			if (SoundAsset.Succeeded())
			{
				FireSound = SoundAsset.Object;
			}
		}
		break;
	case EWeaponType::Rocket:
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/RocketFire.RocketFire"));
			if (SoundAsset.Succeeded())
			{
				FireSound = SoundAsset.Object;
			}
		}
		break;
	}
	
}

void ACGunBase::Reload()
{
	//탄창이 이미 가득 차 있다면 재장전 필요 없음
	if (CurrentAmmo == MaxAmmo)
	{
		return;
	}

	// 탄창을 가득 채움
	CurrentAmmo = MaxAmmo;
}

FVector ACGunBase::GetAimDirection() const
{

	FVector MuzzleLocation = FVector::ZeroVector;
	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	}
	
	//싱글플레이어라 0번을 가져옴
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		return GetActorForwardVector(); 
	}
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
	{
		return GetActorForwardVector();
	}

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
		if (FVector::DistSquared(HitResult.ImpactPoint, MuzzleSpot) < 100.0f) 
		{
			return CameraRotation.Vector();  
		}
		

		return AimDirection;
		
	}

	return CameraRotation.Vector();
}



FVector ACGunBase::SpreadDirection(const FVector OriginDirection) const
{
	float RandomYaw = FMath::RandRange(-GunSpread, GunSpread);
	float RandomPitch = FMath::RandRange(-GunSpread, GunSpread);

	// 원래 방향을 기준으로 랜덤 회전 적용
	FRotator SpreadRotation(RandomPitch, RandomYaw, 0.0f);
	FVector SpreadDirection = SpreadRotation.RotateVector(OriginDirection);
    
	return SpreadDirection.GetSafeNormal(); // 정규화된 방향 반환
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

