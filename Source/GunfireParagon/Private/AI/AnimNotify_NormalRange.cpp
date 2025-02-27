#include "AI/AnimNotify_NormalRange.h"
#include "AI/NormalRangeEnemy.h"

void UAnimNotify_NormalRange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    ANormalRangeEnemy* Enemy = Cast<ANormalRangeEnemy>(MeshComp->GetOwner());
    if (Enemy)
    {
        Enemy->PerformRangeAttack(FVector::ZeroVector);
    }
}
