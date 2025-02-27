#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_BarrageSkill.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_BarrageSkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_BarrageSkill();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnMontageEnded(UBehaviorTreeComponent* OwnerComp);
};
