#include "AI/AnimNotify/AnimNotify_Teleport.h"
#include "AI/Enemy/BossEnemy.h"

void UAnimNotify_Teleport::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;
	
	Boss->ActivateTeleport();
}
