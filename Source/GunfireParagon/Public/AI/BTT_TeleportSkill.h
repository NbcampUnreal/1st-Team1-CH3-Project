#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_TeleportSkill.generated.h"

UCLASS()
class GUNFIREPARAGON_API UBTT_TeleportSkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_TeleportSkill();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnTeleportFinished(UBehaviorTreeComponent* OwnerComp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector TeleportLocationKey;
};
