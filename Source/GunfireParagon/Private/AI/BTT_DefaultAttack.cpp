#include "AI/BTT_DefaultAttack.h"
#include "AIController.h"
#include "AI/BaseEnemy.h"
#include "AI/NormalMeleeEnemy.h"
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

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	FVector LastKnownLocation = BBComp->GetValueAsVector("LastKnownPlayerLocation");
	Enemy->Attack(LastKnownLocation);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

	FTimerHandle AttackTimerHandle;
	Enemy->GetWorldTimerManager().SetTimer(AttackTimerHandle, FTimerDelegate::CreateLambda([this, &OwnerComp]()
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);

			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}), 1.0f, false);

	return EBTNodeResult::InProgress;
}
