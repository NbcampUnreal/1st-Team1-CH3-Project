#include "AI/BTT_FocusTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FocusTarget::UBTT_FocusTarget()
{
	NodeName = TEXT("FocusTarget");
}

EBTNodeResult::Type UBTT_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp) return EBTNodeResult::Failed;

	AActor* TargetPlayer = Cast<AActor>(BBComp->GetValueAsObject("TargetPlayer"));
	if (!TargetPlayer) return EBTNodeResult::Failed;

	AIController->SetFocus(TargetPlayer);

	return EBTNodeResult::Succeeded;
}
