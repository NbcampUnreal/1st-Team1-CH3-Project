#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DefaultAttack.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_DefaultAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_DefaultAttack();

protected:
	UPROPERTY()
	class ABaseEnemy* Enemy;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float Delaseconds) override;
};
