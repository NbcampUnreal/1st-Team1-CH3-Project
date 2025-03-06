#include "AI/AnimNotify/AnimNotify_Heal.h"
#include "AI/Enemy/BossEnemy.h"

void UAnimNotify_Heal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;

	Boss->ActivateHeal();
}
