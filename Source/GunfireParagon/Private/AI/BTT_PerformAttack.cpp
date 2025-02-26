#include "AI/BTT_PerformAttack.h"
#include "AI/BossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_PerformAttack::UBTT_PerformAttack()
{
	NodeName = TEXT("PerformAttack");
}

EBTNodeResult::Type UBTT_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ABossEnemy* Boss = Cast<ABossEnemy>(AIController->GetPawn());
	if (!Boss) return EBTNodeResult::Failed;

	if (Boss->bIsAttacking) return EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	FVector TargetLocation;
	if (BBComp->GetValueAsVector("PlayerLocation") != FVector::ZeroVector)
	{
		TargetLocation = BBComp->GetValueAsVector("PlayerLocation");
	}
	else
	{
		TargetLocation = BBComp->GetValueAsVector("LastKnownPlayerLocation");
	}
	TargetLocation.Z += 75.0f;
	Boss->Attack(TargetLocation);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsAttacking", true);

	Boss->OnSkillMontageEnded.BindUObject(this, &UBTT_PerformAttack::OnMontageEnded, &OwnerComp);

	return EBTNodeResult::InProgress;
}

void UBTT_PerformAttack::OnMontageEnded(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		OwnerComp->GetBlackboardComponent()->SetValueAsBool("IsAttacking", false);
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
