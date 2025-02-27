#include "AI/BTT_DefaultAttack.h"
#include "AI/BaseEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_DefaultAttack::UBTT_DefaultAttack()
{
	NodeName = TEXT("DefaultAttack");
}

EBTNodeResult::Type UBTT_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	if (Enemy->bIsAttacking)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	FVector LastKnownLocation = BBComp->GetValueAsVector("LastKnownPlayerLocation");
	Enemy->Attack(LastKnownLocation);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

	Enemy->OnSkillMontageEnded.RemoveAll(this);
	Enemy->OnSkillMontageEnded.AddUObject(this, &UBTT_DefaultAttack::OnMontageEnded, &OwnerComp);

	return EBTNodeResult::InProgress;
}

void UBTT_DefaultAttack::OnMontageEnded(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
