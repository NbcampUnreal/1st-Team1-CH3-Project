#include "AI/AnimNotify_NormalMelee.h"
#include "AI/NormalMeleeEnemy.h"

void UAnimNotify_NormalMelee::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    ANormalMeleeEnemy* Enemy = Cast<ANormalMeleeEnemy>(MeshComp->GetOwner());
    if (Enemy)
    {
        Enemy->PerformMeleeAttack(FVector::ZeroVector);
    }
}
