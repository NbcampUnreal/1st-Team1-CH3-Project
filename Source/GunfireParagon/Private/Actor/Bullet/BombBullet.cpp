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

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/HitExplosion.HitExplosion"));
	if (SoundAsset.Succeeded())
	{
		ExplosionSound = SoundAsset.Object;
	}
}

void ABombBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABombBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage)
{
	Super::Fire(StartLocation, Direction, GunDamage);
}

void ABombBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBulletOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!OtherActor || OtherActor == this)
		return;
	
	if (!OtherActor || OtherActor->ActorHasTag("Player"))
		return;
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
	if (ExplosionSound.IsValid())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound.Get(), GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("폭발사운드가 없습니다."));
	}
	bool IsHead = false;
	ApplyExplosionDamage();
	OnHitMarker.Broadcast(IsHead);
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
    
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerActor)
	{
		IgnoreActors.Add(PlayerActor);
	}

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

void ABombBullet::OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 벽과 충돌 시 처리
	if (!OtherActor || OtherActor == this) return;
	
	if (OtherActor->ActorHasTag("Bullet")) return;
	if (OtherActor->ActorHasTag("Gun")) return;
	if (OtherActor->ActorHasTag("Player")) return;

	
	UE_LOG(LogTemp, Warning, TEXT("맞은대상: %s"), *OtherActor->GetName());
	if (BulletPool)
	{
		if (ExplosionSound.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound.Get(), GetActorLocation());
			ApplyExplosionDamage();
		}
		BulletPool->ReturnBullet(this, AmmoType);
		SpawnBulletDecal(Hit);
		UE_LOG(LogTemp, Warning, TEXT("총알이 벽과 충돌하여 풀로 반환됨"));
	}
	
	
}
