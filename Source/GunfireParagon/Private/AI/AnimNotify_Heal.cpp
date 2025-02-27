#include "AI/AnimNotify_Heal.h"
#include "AI/BossEnemy.h"

void UAnimNotify_Heal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;

	Boss->ActivateHeal();
}
