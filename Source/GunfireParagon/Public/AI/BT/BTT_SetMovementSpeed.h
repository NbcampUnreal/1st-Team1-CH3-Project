#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_SetMovementSpeed.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_SetMovementSpeed : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_SetMovementSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
