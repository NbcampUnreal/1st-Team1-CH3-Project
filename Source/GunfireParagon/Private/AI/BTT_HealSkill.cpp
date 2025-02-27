#include "AI/BTT_HealSkill.h"
#include "AI/BossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_HealSkill::UBTT_HealSkill()
{
	NodeName = TEXT("Heal Skill");
}

EBTNodeResult::Type UBTT_HealSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;
    if (Boss->bIsAttacking) return EBTNodeResult::Failed;

    Boss->UseHealSkill();

    OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

    Boss->OnSkillMontageEnded.RemoveAll(this);
    Boss->OnSkillMontageEnded.AddUObject(this, &UBTT_HealSkill::OnHealFinished, &OwnerComp);

    return EBTNodeResult::InProgress;
}

void UBTT_HealSkill::OnHealFinished(UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp)
    {
        OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
        OwnerComp->GetBlackboardComponent()->SetValueAsBool("ShouldHealSkill", false);
        
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}
