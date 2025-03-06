#include "AI/AnimNotify/AnimNotify_NormalMelee.h"
#include "AI/Enemy/NormalMeleeEnemy.h"

void UAnimNotify_NormalMelee::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    ANormalMeleeEnemy* Enemy = Cast<ANormalMeleeEnemy>(MeshComp->GetOwner());
    if (Enemy)
    {
        Enemy->PerformMeleeAttack(FVector::ZeroVector);
    }
}
