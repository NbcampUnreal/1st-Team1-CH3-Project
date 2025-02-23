#include "AI/BTT_FocusClear.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FocusClear::UBTT_FocusClear()
{
	NodeName = TEXT("FocusClear");
}

EBTNodeResult::Type UBTT_FocusClear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (BBComp)
	{
		BBComp->SetValueAsBool("PauseLastKnownUpdate", true);

		FTimerHandle TimerHandle;
		AIController->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [BBComp]()
			{
				BBComp->SetValueAsBool("PauseLastKnownUpdate", false);
			}, 0.5f, false);
	}

	return EBTNodeResult::Succeeded;
}
