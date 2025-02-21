#include "AI/BTT_DefaultAttack.h"
#include "AIController.h"
#include "AI/BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_DefaultAttack::UBTT_DefaultAttack()
{
	NodeName = TEXT("DefaultAttack");
}

EBTNodeResult::Type UBTT_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	if (Enemy->bIsAttacking)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->Attack();

	bNotifyTick = true;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

	return EBTNodeResult::InProgress;
}

void UBTT_DefaultAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float Delaseconds)
{
	if (!Enemy) return;

	if (!Enemy->bIsAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack Finished for: %s"), *Enemy->GetName());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
