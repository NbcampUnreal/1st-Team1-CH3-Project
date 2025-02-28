#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_HealSkill.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_HealSkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_HealSkill();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnHealFinished(UBehaviorTreeComponent* OwnerComp);
};
