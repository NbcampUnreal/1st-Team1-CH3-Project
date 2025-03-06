#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FocusClear.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_FocusClear : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FocusClear();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
