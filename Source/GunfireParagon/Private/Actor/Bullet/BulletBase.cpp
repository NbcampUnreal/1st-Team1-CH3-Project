

#include "Actor/Bullet/BulletBase.h"
#include "Actor/BulletPool.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

FOnHitMarkerDele ABulletBase::OnHitMarker;

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if(!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(15.0f);

		// 연속 충돌 감지(CCD) 활성화 (고속 총알이 벽을 통과하는 문제 해결)
		CollisionComponent->SetNotifyRigidBodyCollision(true);
		CollisionComponent->SetSimulatePhysics(false);
		

		// 루트 컴포넌트가 콜리전 컴포넌트가 되도록 설정합니다.
		RootComponent = CollisionComponent;
	}


	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(CollisionComponent);

	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	ProjectileMovement->InitialSpeed = 2000.0f; // 기본 속도 설정
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 방향 따라 회전
	ProjectileMovement->bShouldBounce = false; // 튕김 여부
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력여부 
	ProjectileMovement->bSweepCollision = true; //총알이 벽을 지나치는 문제 해결

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/headshotSound.headshotSound"));

	if (SoundAsset.Succeeded())
	{
		HeadHitSound = SoundAsset.Object;
	}
	else
	{
	}
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileMovement)
	{
		ProjectileMovement->Deactivate(); //총알이 풀에서 생성될 때 기본적으로 멈추도록 설정
	}
	

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBulletOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABulletBase::OnBulletHit);


	BulletPool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
	
	
}

void ABulletBase::Fire(FVector StartLocation, FVector Direction, float GunDamage,float GunSpeed)
{
	
	SetActorLocation(StartLocation);
	SetActorRotation(Direction.Rotation());
	BulletDamage = GunDamage;

	if (!ProjectileMovement->UpdatedComponent)
	{
		ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	}
	if (ProjectileMovement)
	{

		ProjectileMovement->StopMovementImmediately();
        
		ProjectileMovement->InitialSpeed = GunSpeed;
		ProjectileMovement->MaxSpeed = GunSpeed;

		ProjectileMovement->Velocity = Direction * GunSpeed;
		ProjectileMovement->Activate();
	}
}

void ABulletBase::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (OtherActor->ActorHasTag("Bullet")) return;
	if (OtherActor->ActorHasTag("Gun")) return;


	if (OtherActor->ActorHasTag("Enemy"))
	{
		bool IsHead = false;
		float FinalDamage = BulletDamage;
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			if (HeadHitSound.IsValid())
			{
				UGameplayStatics::PlaySoundAtLocation(this, HeadHitSound.Get(), GetActorLocation());
			}
			IsHead = true;
			FinalDamage *= 2.0f;
		}
		else
		{
		}
		SpawnBulletDecal(SweepResult);
		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());
		OnHitMarker.Broadcast(IsHead);
	}
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
FVector NormalImpulse, const FHitResult& Hit)
{
	
	// 벽과 충돌 시 처리
	if (!OtherActor || OtherActor == this) return;
	
	if (OtherActor->ActorHasTag("Bullet")) return;
	if (OtherActor->ActorHasTag("Gun")) return;
	
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
		SpawnBulletDecal(Hit);
	}

	
}

void ABulletBase::SpawnBulletDecal(const FHitResult& Hit)
{
	if (!BulletDecalMaterial)
	{
		return;
	} 
	UDecalComponent* BulletDecal = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(), 
		BulletDecalMaterial, 
		DecalSize,
		Hit.ImpactPoint, 
		Hit.ImpactNormal.Rotation(), 
		DecalLifeTime
	);

	if (BulletDecal)
	{
		BulletDecal->SetFadeScreenSize(0.001f); // 거리별 페이드 효과 적용 (선택사항)
	}
}

