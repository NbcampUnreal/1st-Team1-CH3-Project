#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_SetRandomLocation.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTD_SetRandomLocation : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTD_SetRandomLocation();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius = 500.0f;
	
};
