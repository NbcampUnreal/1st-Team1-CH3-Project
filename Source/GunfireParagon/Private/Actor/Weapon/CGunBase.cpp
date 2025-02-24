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

	CurrentAmmo = MaxAmmo;
	bCanFire =true;
	
	
}

void ACGunBase::BeginPlay()
{
	Super::BeginPlay();

	if (GunMesh && GunMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = GunMesh->GetSocketLocation(TEXT("Muzzle"));
		UE_LOG(LogTemp, Warning, TEXT("총구 위치 설정됨: %s"), *MuzzleSpot.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GunMesh에 'Muzzle' 소켓이 존재하지 않습니다. 기본 위치 사용."));
	}
	
	BulletPool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
	
}

void ACGunBase::Fire()
{
	if (!CanFire()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("발사 불가! 탄약 부족 또는 발사 타이머 동작 중"));
		return;
	}

	//  발사 후 탄약 감소
	CurrentAmmo--;
	
	if (GunMesh && GunMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		MuzzleSpot = GunMesh->GetSocketLocation(TEXT("Muzzle"));
	}
	
	// 지금 잏시적으로 앞으로 발사하게만해둠
	FVector forwardDirection = GetAimDirection();
	
	//  총알을 풀에서 가져오기
	ABulletBase* Bullet = BulletPool->GetPooledBullet(AmmoType);
	if (Bullet)
	{
		Bullet->Fire(MuzzleSpot, forwardDirection, Damage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("사용 가능한 총알이 없습니다!"));
	}

	bCanFire = false;
	GetWorldTimerManager().SetTimer(FireTimer, this, &ACGunBase::SetIsFire, GunDelay-0.01f, false);

	
	//지금로직은 마우스를 누르고있을때가 아닌 누를때의 바인딩과 뗏을때의 바인딩을 설정해야함.
	//자동 발사 모드일 경우 타이머로 반복 실행
	if (bIsAutoFire)
	{
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &ACGunBase::Fire, GunDelay-0.01f, false);
	}
}

bool ACGunBase::CanFire() const
{
	//  bCanFire가 false면 발사 불가능 (GunDelay 동안 발사 막기)
	if (!bCanFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("발사 대기 중!"));
		return false;
	}

	// 탄약이 없으면 발사 불가능
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
	// 탄창이 이미 가득 차 있다면 재장전 필요 없음
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
	//싱글플레이어라 0번을 가져옴
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return GetActorForwardVector(); // 기본값: 총이 향하는 방향

	//  2. 플레이어의 컨트롤러 가져오기
	AMyPlayerController* PlayerController = Cast<AMyPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController) return GetActorForwardVector();
	FVector WorldLocation, WorldDirection;
	if (PlayerController->DeprojectScreenPositionToWorld(960.0f, 540.0f, WorldLocation, WorldDirection))
	{
		//  1. 화면 중앙(960x540, 1920x1080 해상도 기준)에서 월드 방향 벡터 얻기

		FVector TraceStart = PlayerController->PlayerCameraManager->GetCameraLocation();
		FVector TraceEnd = TraceStart + (WorldDirection * 10000.0f); // 긴 레이 트레이스

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner()); // 플레이어 자신 무시

		// 2. 라인 트레이스 실행
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			// 3. 충돌한 위치로 발사 방향 계산
			FVector AimDirection = (HitResult.ImpactPoint - MuzzleSpot).GetSafeNormal();
			return AimDirection;
		}
	}

	// 실패 시 기본 총구 방향
	return GetActorForwardVector();
}

FVector ACGunBase::SpreadDirection(const FVector OriginDirection) const
{
	float RandomYaw = FMath::RandRange(-GunSpread, GunSpread);
	float RandomPitch = FMath::RandRange(-GunSpread, GunSpread);

	FRotator SpreadRotation = FRotator(RandomPitch, RandomYaw, 0.0f);
	return SpreadRotation.RotateVector(OriginDirection);
}



