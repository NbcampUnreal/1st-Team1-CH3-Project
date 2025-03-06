#include "AI/AnimNotify/AnimNotify_MeteorProjectile.h"
#include "AI/Enemy/BossEnemy.h"

void UAnimNotify_MeteorProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;

	Boss->SpawnMeteor();
}
