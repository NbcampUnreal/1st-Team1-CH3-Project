// Fill out your copyright notice in the Description page of Project Settings.


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

	// 탄환 타입별로 풀을 생성
	//  탄환 타입별로 풀을 생성 (블루프린트 클래스를 사용하여 스폰)
	for (int32 i = 0; i < PoolSize; i++)
	{
		if (NormalBulletClass) // 블루프린트 클래스가 설정된 경우에만 생성
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
		if (NormalBulletClass) // 블루프린트 클래스가 설정된 경우에만 생성
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
		if (NormalBulletClass) // 블루프린트 클래스가 설정된 경우에만 생성
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
	
	//GetWorldTimerManager().SetTimer(BulletCheckTimer, this, &ABulletPool::CheckBulletLifetime, MaxBulletLifetime, true);
}
//void ABulletPool::CheckBulletLifetime()
//{
//	float CurrentTime = GetWorld()->GetTimeSeconds();
//
//	if (ActiveBullets.Num() == 0) // 총알이 없으면 검사하지 않음
//	{
//        UE_LOG(LogTemp, Warning, TEXT("활성화된 총알 없음"));
//		return;
//	}
//	TArray<ABulletBase*> BulletsToRemove; // 제할 총알을 따로 저장
//    
//        for (ABulletBase* Bullet : ActiveBullets)
//        {
//        	if (!IsValid(Bullet)) // nullptr 또는 삭제된 경우 체크
//        	{
//        		BulletsToRemove.Add(Bullet);
//        		continue;
//        	}
//    
//        	//  총알이 일정 시간 이상 유지되었다면 회수
//        	if (CurrentTime - Bullet->GetSpawnTime() >= MaxBulletLifetime)
//        	{
//        		ReturnBullet(Bullet, Bullet->AmmoType);
//        		BulletsToRemove.Add(Bullet);
//        		UE_LOG(LogTemp, Warning, TEXT("너무 오래있던 총알 회수"));
//        	}
//        }
//    
//        //  반복문 종료 후 한꺼번에 삭제 (Set에서 안전하게 제거)
//        if (BulletsToRemove.Num() > 0)
//        {
//        	for (ABulletBase* Bullet : BulletsToRemove)
//        	{
//        		if (ActiveBullets.Contains(Bullet))
//        		{
//        			ActiveBullets.Remove(Bullet);
//        		}
//        	}
//        }
//
//}


ABulletBase* ABulletPool::GetPooledBullet(EAmmoType AmmoType)
{
	UE_LOG(LogTemp, Warning, TEXT("총알 풀에서 탄환 가져오기 시도! AmmoType: %d"), (int32)AmmoType);

	if (!BulletPools.Contains(AmmoType))
	{
		UE_LOG(LogTemp, Error, TEXT("탄환 풀에 AmmoType이 존재하지 않음!"));
		return nullptr;
	}

	for (ABulletBase* Bullet : BulletPools[AmmoType])
	{
		if (!Bullet->IsActorTickEnabled() && Bullet->IsHidden()) // 사용 중이 아닌 총알 찾기
		{
			Bullet->SetActorEnableCollision(true);
			Bullet->SetActorHiddenInGame(false);
			Bullet->SetActorTickEnabled(true);
			Bullet->SetActorRotation(FRotator::ZeroRotator); // 🔄 회전 초기화
			Bullet->ProjectileMovement->Velocity = FVector::ZeroVector; // 🔄 속도 초기화
			Bullet->ProjectileMovement->StopMovementImmediately();
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
		UE_LOG(LogTemp, Error, TEXT("잘못된 탄환 타입!"));
		return nullptr;
	}

	if (NewBullet)
	{
		BulletPools[AmmoType].Add(NewBullet);
		UE_LOG(LogTemp, Warning, TEXT("새로운 탄환 생성 완료: %s"), *NewBullet->GetName());
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
			Bullet->ProjectileMovement->SetUpdatedComponent(nullptr);
		}
		
		if (ActiveBullets.Contains(Bullet))
		{
			ActiveBullets.Remove(Bullet);
		}
        UE_LOG(LogTemp, Warning, TEXT("총알이 풀로 반환됨: %s"), *Bullet->GetName());
		
	}
}


