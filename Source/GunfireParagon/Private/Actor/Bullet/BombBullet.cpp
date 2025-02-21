// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Bullet/BombBullet.h"
#include "Actor/BulletPool.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"

ABombBullet::ABombBullet()
{
	AmmoType = EAmmoType::Bomb;
	BulletDamage = 100.0f;
	ExplosionRadius = 300.0f;
}

void ABombBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABombBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage)
{
	Super::Fire(StartLocation, Direction, GunDamage);
	
	SetActorLocation(StartLocation);
	BulletDamage = GunDamage;
    
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void ABombBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBulletOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!OtherActor || OtherActor == this)
		return;

	ApplyExplosionDamage();

	// 폭발 후 총알을 풀로 반환
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
	}
	else
	{
		Destroy();
	}
}

void ABombBullet::ApplyExplosionDamage()
{
	if (!GetWorld()) return;

	TArray<AActor*> IgnoreActors;
    
	// 플레이어를 찾아서 예외 목록에 추가
	// AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	// if (PlayerActor)
	// {
	// 	IgnoreActors.Add(PlayerActor);
	// }

	// 플레이어를 제외한 모든 적에게 폭발 데미지 적용
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		BulletDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoreActors,             // 플레이어 제외
		this,
		GetInstigatorController(),
		true
	);

}
