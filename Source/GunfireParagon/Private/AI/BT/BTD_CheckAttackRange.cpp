#include "AI/BT/BTD_CheckAttackRange.h"
#include "AI/Enemy/BaseEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_CheckAttackRange::UBTD_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
}

bool UBTD_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;

	if (BlackboardComponent->GetValueAsBool("IsAttacking")) return false;

	AActor* TargetPlayer = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetPlayer"));
	if (!TargetPlayer || !TargetPlayer->ActorHasTag("Player")) return false;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return false;

	float DistanceToPlayer = FVector::Dist(Enemy->GetActorLocation(), TargetPlayer->GetActorLocation());

	bool bCanAttack = DistanceToPlayer <= Enemy->AttackRange;

	return bCanAttack;
}
