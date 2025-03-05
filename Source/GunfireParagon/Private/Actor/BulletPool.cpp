

#include "Actor/BulletPool.h" 
#include "Actor/Bullet/BulletBase.h" 
#include "Actor/Bullet/NormalBullet.h" // cpp에서만 포함하여 불필요한 의존성을 줄임

ABulletPool::ABulletPool()
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void ABulletPool::BeginPlay()
{
	Super::BeginPlay();

}

ABulletBase* ABulletPool::GetPooledBullet(EAmmoType AmmoType)
{
	if (!BulletPools.Contains(AmmoType))
	{
		return nullptr;
	}
	for (ABulletBase* Bullet : BulletPools[AmmoType])
	{
		if (!Bullet->IsActorTickEnabled() && Bullet->IsHidden()) // 사용 중이 아닌 총알 찾기
		{
			Bullet->SetActorEnableCollision(true);
			Bullet->SetActorHiddenInGame(false);
			Bullet->SetActorTickEnabled(true);
			Bullet->SetActorRotation(FRotator::ZeroRotator); // 회전 초기화
			Bullet->ProjectileMovement->Velocity = FVector::ZeroVector; // 속도 초기화
			Bullet->ProjectileMovement->StopMovementImmediately();
			Bullet->ProjectileMovement->bSimulationEnabled = true;
			if (Bullet->ProjectileMovement)
			{
				Bullet->ProjectileMovement->bSimulationEnabled = true;
				Bullet->ProjectileMovement->SetUpdatedComponent(Bullet->CollisionComponent);
			}
			return Bullet;
		}
	}
	// 새 총알 생성
	ABulletBase* NewBullet = nullptr;
	switch (AmmoType)
	{
	case EAmmoType::Normal:
		NewBullet = GetWorld()->SpawnActor<ANormalBullet>(ANormalBullet::StaticClass());
		break;
	default:
		return nullptr;
	}
	if (NewBullet)
	{
		BulletPools[AmmoType].Add(NewBullet);
	}
	return NewBullet;
}
void ABulletPool::ReturnBullet(ABulletBase* Bullet, EAmmoType AmmoType)
{
	if (Bullet)
	{
		Bullet->SetActorEnableCollision(false);
		Bullet->SetActorHiddenInGame(true);
		Bullet->SetActorTickEnabled(false); //  총알 비활성화
		Bullet->SetActorLocation(FVector(0, 0, -1000)); // 안 보이는 곳으로 이동, 일종의 꼼수
		Bullet->SetActorRotation(FRotator::ZeroRotator);

		if (Bullet->ProjectileMovement)
		{
			Bullet->ProjectileMovement->StopMovementImmediately();
			Bullet->ProjectileMovement->Velocity = FVector::ZeroVector;
		}
		if (ActiveBullets.Contains(Bullet))
		{
			ActiveBullets.Remove(Bullet);
		}
		
	}
}

void ABulletPool::InitializePooled()
{
	for (int32 i = 0; i < PoolSize; i++)
	{
		if (NormalBulletClass)
		{
			ABulletBase* NormalBullet = GetWorld()->SpawnActor<ABulletBase>(NormalBulletClass);
			if (NormalBullet)
			{
				NormalBullet->SetActorEnableCollision(false);
				NormalBullet->SetActorHiddenInGame(true);
				NormalBullet->SetActorTickEnabled(false);
				NormalBullet->ProjectileMovement->StopMovementImmediately();
				BulletPools.FindOrAdd(EAmmoType::Normal).Add(NormalBullet);
			}
		}
		if (NormalBulletClass) 
		{
			ABulletBase* PierceBullet = GetWorld()->SpawnActor<ABulletBase>(PierceBulletClass);
			if (PierceBullet)
			{
				PierceBullet->SetActorEnableCollision(false);
				PierceBullet->SetActorHiddenInGame(true);
				PierceBullet->SetActorTickEnabled(false);
				PierceBullet->ProjectileMovement->StopMovementImmediately();
				BulletPools.FindOrAdd(EAmmoType::Pierce).Add(PierceBullet);
			}
		}
		if (NormalBulletClass) 
		{
			ABulletBase* BombBullet = GetWorld()->SpawnActor<ABulletBase>(BombBulletClass);
			if (BombBullet)
			{
				BombBullet->SetActorEnableCollision(false);
				BombBullet->SetActorHiddenInGame(true);
				BombBullet->SetActorTickEnabled(false);
				BombBullet->ProjectileMovement->StopMovementImmediately();
				BulletPools.FindOrAdd(EAmmoType::Bomb).Add(BombBullet);
			}
		}
	}
}


