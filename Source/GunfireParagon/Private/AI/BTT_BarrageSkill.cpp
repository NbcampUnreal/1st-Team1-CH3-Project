#include "AI/BTT_BarrageSkill.h"
#include "AI/BossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_BarrageSkill::UBTT_BarrageSkill()
{
	NodeName = TEXT("Barrage Skill");
}

EBTNodeResult::Type UBTT_BarrageSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;

    if (Boss->bIsAttacking) return EBTNodeResult::Failed;

    Boss->UseBarrageSkill();

    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

    Boss->OnSkillMontageEnded.RemoveAll(this);
    Boss->OnSkillMontageEnded.AddUObject(this, &UBTT_BarrageSkill::OnMontageEnded, &OwnerComp);

    return EBTNodeResult::InProgress;
}

void UBTT_BarrageSkill::OnMontageEnded(UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp)
    {
        OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}