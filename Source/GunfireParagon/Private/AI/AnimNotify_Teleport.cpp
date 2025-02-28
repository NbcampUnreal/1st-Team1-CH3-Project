#include "AI/AnimNotify_Teleport.h"
#include "AI/BossEnemy.h"

void UAnimNotify_Teleport::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(MeshComp->GetOwner());
	if (!Boss) return;
	
	Boss->ActivateTeleport();
}
