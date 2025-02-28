#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckAttackRange.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTD_CheckAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTD_CheckAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
