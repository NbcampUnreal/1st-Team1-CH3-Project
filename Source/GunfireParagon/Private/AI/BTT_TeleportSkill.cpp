#include "AI/BTT_TeleportSkill.h"
#include "AI/BossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_TeleportSkill::UBTT_TeleportSkill()
{
    NodeName = TEXT("Teleport Skill");
}

EBTNodeResult::Type UBTT_TeleportSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;
    if (Boss->bIsAttacking) return EBTNodeResult::Failed;

    FVector TeleportPoint = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TeleportLocationKey.SelectedKeyName);
    Boss->UseTeleportSkill(TeleportPoint);

    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

    Boss->OnSkillMontageEnded.BindUObject(this, &UBTT_TeleportSkill::OnTeleportFinished, &OwnerComp);

    return EBTNodeResult::InProgress;
}

void UBTT_TeleportSkill::OnTeleportFinished(UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp)
    {
        OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}
