#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ClearBlackboardKey.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_ClearBlackboardKey : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_ClearBlackboardKey();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
