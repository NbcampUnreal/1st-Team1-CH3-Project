// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Bullet/NormalBullet.h"
#include "Actor/BulletPool.h"
#include "Kismet/GameplayStatics.h"

ANormalBullet::ANormalBullet()
{
	AmmoType = EAmmoType::Normal;
}

void ANormalBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ANormalBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage)
{
	SetActorLocation(StartLocation);
	SetActorRotation(Direction.Rotation());
	BulletDamage = GunDamage;

	if (!ProjectileMovement)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ ProjectileMovement가 nullptr!"));
		return;
	}
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		UE_LOG(LogTemp, Warning, TEXT("✅ CollisionComponent가 활성화됨"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ CollisionComponent가 nullptr!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("🚀 Fire() 실행 - 위치: %s, 방향: %s"), *StartLocation.ToString(), *Direction.ToString());

	// ✅ 추가: ProjectileMovement가 비활성화 상태인지 확인
	if (!ProjectileMovement->bSimulationEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ ProjectileMovement가 비활성화 상태! 다시 활성화함."));
		ProjectileMovement->bSimulationEnabled = true;
		ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	}

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
	ProjectileMovement->Activate();

	// ✅ 추가: 속도 확인
	if (ProjectileMovement->Velocity.IsNearlyZero())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Velocity가 (0,0,0)!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("✅ 총알 발사! 속도: %s"), *ProjectileMovement->Velocity.ToString());
	}
}

UE_DISABLE_OPTIMIZATION
void ANormalBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	Super::OnBulletOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!OtherActor || OtherActor == this) 
	{
		UE_LOG(LogTemp, Warning, TEXT("자신충돌했으나 무시함"));
		return;
	}
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
	
	float FinalDamage = BulletDamage;
	
	if (OtherActor->ActorHasTag("Monster"))
	{
		// 맞은 부위가 "head"인지 확인 (Skeleton Bone Name 사용)
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			FinalDamage *= 2.0f; // 헤드샷 데미지 2배
			UE_LOG(LogTemp, Warning, TEXT("헤드샷! 데미지: %f"), FinalDamage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("일반 공격! 데미지: %f"), FinalDamage);
		}
		//탄흔
		SpawnBulletDecal(SweepResult);
		//  ApplyPointDamage 사용 (맞은 위치 포함)
		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());

		UE_LOG(LogTemp, Warning, TEXT("총알이 Enemy를 맞춤: %s"), *OtherActor->GetName());
	}

	
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
		UE_LOG(LogTemp, Warning, TEXT("%s 총알이 %s 맞춤"),*GetName() ,*OtherActor->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Normal Bullet__풀링으로 반환됨"));
	}
	
	
	
}
UE_ENABLE_OPTIMIZATION

