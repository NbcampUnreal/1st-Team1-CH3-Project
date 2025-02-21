#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateDistance.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTService_UpdateDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
