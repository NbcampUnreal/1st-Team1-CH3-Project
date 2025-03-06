#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FocusTarget.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_FocusTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_FocusTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
