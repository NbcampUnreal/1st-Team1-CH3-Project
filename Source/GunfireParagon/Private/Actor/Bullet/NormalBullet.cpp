
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

void ANormalBullet::Fire(FVector StartLocation, FVector Direction, float GunDamage,float GunSpeed)
{
	SetActorLocation(StartLocation);
	SetActorRotation(Direction.Rotation());
	BulletDamage = GunDamage;

	if (!ProjectileMovement)
	{
		return;
	}
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (!ProjectileMovement->bSimulationEnabled)
	{
		ProjectileMovement->bSimulationEnabled = true;
		ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	}

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
	ProjectileMovement->Activate();

}

//UE_DISABLE_OPTIMIZATION
void ANormalBullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) 
	{
		return;
	}
	if (OtherActor->ActorHasTag("Bullet"))
	{
		return;
	}
	if (OtherActor->ActorHasTag("Gun"))
	{
		return;
	}
	
	float FinalDamage = BulletDamage;
	
	if (OtherActor->ActorHasTag("Monster"))
	{
		bool IsHead = false;
		if (SweepResult.BoneName == "head" || SweepResult.BoneName == "Head")
		{
			FinalDamage *= 2.0f; // 헤드샷 데미지 2배

			if (HeadHitSound.IsValid())
			{
				UGameplayStatics::PlaySoundAtLocation(this, HeadHitSound.Get(), GetActorLocation());
			}
			IsHead = true;
		}
		//탄흔
		SpawnBulletDecal(SweepResult);
		//  ApplyPointDamage 사용 (맞은 위치 포함)
		UGameplayStatics::ApplyPointDamage(OtherActor, FinalDamage, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());
		OnHitMarker.Broadcast(IsHead);
	}

	
	if (BulletPool)
	{
		BulletPool->ReturnBullet(this, AmmoType);
	}
	
	
	
}
//UE_ENABLE_OPTIMIZATION

