#include "AI/AnimNotify/AnimNotify_NormalRange.h"
#include "AI/Enemy/NormalRangeEnemy.h"

void UAnimNotify_NormalRange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    ANormalRangeEnemy* Enemy = Cast<ANormalRangeEnemy>(MeshComp->GetOwner());
    if (!Enemy) return;
    
    Enemy->PerformRangeAttack(FVector::ZeroVector);
    
}
