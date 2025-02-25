#include "AI/AnimNotify_EndAttack.h"
#include "AI/BaseEnemy.h"

void UAnimNotify_EndAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

    ABaseEnemy* Enemy = Cast<ABaseEnemy>(MeshComp->GetOwner());
    if (Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("EndAttack Called from: %s, Mesh: %s"),*Enemy->GetName(), *MeshComp->GetName());

        if (!Enemy->bIsAttacking) return;

        Enemy->EndAttack();
    }
}
