#include "AI/AnimNotify_EliteRange.h"
#include "AI/EliteRangeEnemy.h"

void UAnimNotify_EliteRange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AEliteRangeEnemy* Enemy = Cast<AEliteRangeEnemy>(MeshComp->GetOwner());
	if (!Enemy) return;


	Enemy->PerformRangeAttack(FVector::ZeroVector);
}
