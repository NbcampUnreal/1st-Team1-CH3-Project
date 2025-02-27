#include "AI/AnimNotify_FireProjectile.h"
#include "AI/BossEnemy.h"

void UAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;

	FVector TargetLocation = Boss->GetPlayerLocation();

	Boss->PerformRangeAttack(TargetLocation);
}
