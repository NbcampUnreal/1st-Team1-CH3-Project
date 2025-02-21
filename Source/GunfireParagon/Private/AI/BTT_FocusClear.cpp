#include "AI/BTT_FocusClear.h"
#include "AIController.h"

UBTT_FocusClear::UBTT_FocusClear()
{
	NodeName = TEXT("FocusClear");
}

EBTNodeResult::Type UBTT_FocusClear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	return EBTNodeResult::Type();
}
