#include "AI/AnimNotify/AnimNotify_EliteRange.h"
#include "AI/Enemy/EliteRangeEnemy.h"

void UAnimNotify_EliteRange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AEliteRangeEnemy* Enemy = Cast<AEliteRangeEnemy>(MeshComp->GetOwner());
	if (!Enemy) return;


	Enemy->PerformRangeAttack(FVector::ZeroVector);
}
