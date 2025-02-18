#include "CGunBase.h"

ACGunBase::ACGunBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
}

void ACGunBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACGunBase::Fire(FVector StartPos, FVector Direction, float Damage)
{
}

FVector ACGunBase::SpreadDirection(const FVector OriginDirection) const
{
	float RandomYaw = FMath::RandRange(-GunSpread, GunSpread);
	float RandomPitch = FMath::RandRange(-GunSpread, GunSpread);

	FRotator SpreadRotation = FRotator(RandomPitch, RandomYaw, 0.0f);
	return SpreadRotation.RotateVector(OriginDirection);
}



