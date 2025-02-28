#include "AI/BTT_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_ClearBlackboardKey::UBTT_ClearBlackboardKey()
{
	NodeName = TEXT("Clear Blackboard Value");
}

EBTNodeResult::Type UBTT_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());

	return EBTNodeResult::Succeeded;
}
