

#include "Actor/Bullet/BulletBase.h"
#include "Actor/BulletPool.h"
#include "Kismet/GameplayStatics.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if(!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(15.0f);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionComponent->SetCollisionObjectType(ECC_PhysicsBody);
		CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 적과 충돌 가능하게


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
	
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileMovement)
	{
		ProjectileMovement->Deactivate(); //총알이 풀에서 생성될 때 기본적으로 멈추도록 설정
	}

	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); //적과만 충돌
	CollisionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore); //물리 오브젝트 무시
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);//벽만 충돌 가능

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnBulletOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABulletBase::OnBulletHit);


	BulletPool = Cast<ABulletPool>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletPool::StaticClass()));
	if (!BulletPool)
	{
		UE_LOG(LogTemp, Error, TEXT("BulletPool을 찾을 수 없습니다!"));
	}
	
}

void ABulletBase::Fire(FVector StartLocation, FVector Direction, float GunDamage)
{
}

void ABulletBase::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (OtherActor->ActorHasTag("Bullet"))
	{
		return;
	}

	if (OtherActor->ActorHasTag("Gun"))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("총알이 %s을(를) 맞췄음!"), *OtherActor->GetName());

	if (OtherActor->ActorHasTag("Enemy"))
	{
		float FinalDamage = BulletDamage;
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			FinalDamage *= 2.0f;
			UE_LOG(LogTemp, Warning, TEXT("헤드샷! 데미지: %f"), FinalDamage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("일반 공격! 데미지: %f"), FinalDamage);
		}

		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());
	}
}

void ABulletBase::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
FVector NormalImpulse, const FHitResult& Hit)
{
	
	// 벽과 충돌 시 처리
	if (!OtherActor || OtherActor == this) return;
	
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

	
	UE_LOG(LogTemp, Warning, TEXT("총알이 벽과 충돌: %s"), *OtherActor->GetName());
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, EAmmoType::Normal);
		UE_LOG(LogTemp, Warning, TEXT("총알이 벽과 충돌하여 풀로 반환됨"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("총알 삭제됨"));
		Destroy();
	}
}

void ABulletBase::SpawnBulletDecal(const FHitResult& Hit)
{
	if (!BulletDecalMaterial) return; 

	// 데칼 크기 설정
	FVector DecalSize = FVector(0.2f, 0.2f, 0.2f);  

	// 탄흔을 남길 위치와 방향 설정
	UDecalComponent* BulletDecal = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(), 
		BulletDecalMaterial, 
		DecalSize,
		Hit.ImpactPoint, 
		Hit.ImpactNormal.Rotation(), 
		10.0f 
	);

	if (BulletDecal)
	{
		BulletDecal->SetFadeScreenSize(0.001f); // 거리별 페이드 효과 적용 (선택사항)
	}
}

