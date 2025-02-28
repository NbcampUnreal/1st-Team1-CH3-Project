#include "AI/BTT_MeteorSkill.h"
#include "AI/BossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MeteorSkill::UBTT_MeteorSkill()
{
	NodeName = TEXT("Meteor Skill");
}

EBTNodeResult::Type UBTT_MeteorSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;

    if (Boss->bIsAttacking) return EBTNodeResult::Failed;

    Boss->UseMeteorSkill();

    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

    Boss->OnSkillMontageEnded.RemoveAll(this);
    Boss->OnSkillMontageEnded.AddUObject(this, &UBTT_MeteorSkill::OnMontageEnded, &OwnerComp);
   
    return EBTNodeResult::InProgress;
}

void UBTT_MeteorSkill::OnMontageEnded(UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp)
    {
        OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}
