#include "Actor/Trap/TrapBase.h"
#include "Kismet/GameplayStatics.h"

ATrapBase::ATrapBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATrapBase::BeginPlay()
{
	Super::BeginPlay();

	ActivateTrap();
}

void ATrapBase::ActivateTrap()
{
}

void ATrapBase::OnTrapTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		ApplyDamage(OtherActor);
	}
}

void ATrapBase::ApplyDamage(AActor* Target)
{
	if (Target && bIsActive)
	{
		UGameplayStatics::ApplyDamage(Target, DamageAmount, nullptr, this, nullptr);
	}
}