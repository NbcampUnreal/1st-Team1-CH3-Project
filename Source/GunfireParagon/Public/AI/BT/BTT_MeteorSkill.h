#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MeteorSkill.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_MeteorSkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_MeteorSkill();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void OnMontageEnded(UBehaviorTreeComponent* OwnerComp);
};
