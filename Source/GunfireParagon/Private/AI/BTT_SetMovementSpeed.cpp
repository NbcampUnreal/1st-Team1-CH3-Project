#include "AI/BTT_SetMovementSpeed.h"
#include "AIController.h"
#include "AI/BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed()
{
	NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UBlackboardComponent* BBcomp = OwnerComp.GetBlackboardComponent();
    if (!BBcomp) return EBTNodeResult::Failed;

    bool bHasSpottedPlayer = BBcomp->GetValueAsBool(TEXT("HasSpottedPlayer"));

    float NewSpeed = bHasSpottedPlayer ? Enemy->BaseWalkSpeed : Enemy->BaseWalkSpeed * 0.5f;
    Enemy->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

    return EBTNodeResult::Succeeded;
}
