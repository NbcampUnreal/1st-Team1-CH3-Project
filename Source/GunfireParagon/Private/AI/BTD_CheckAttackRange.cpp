#include "AI/BTD_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/BaseEnemy.h"

UBTD_CheckAttackRange::UBTD_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
}

bool UBTD_CheckAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return false;

	AActor* TargetPlayer = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetPlayer"));
	if (!TargetPlayer || !TargetPlayer->ActorHasTag("Player")) return false;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return false;

	float DistanceToPlayer = FVector::Dist(Enemy->GetActorLocation(), TargetPlayer->GetActorLocation());

	bool bCanAttack = DistanceToPlayer <= Enemy->AttackRange;
	UE_LOG(LogTemp, Warning, TEXT("CheckAttackRange: %s, Distance: %f, AttackRange: %f"), bCanAttack ? TEXT("True") : TEXT("False"), DistanceToPlayer, Enemy->AttackRange);

	return bCanAttack;
}
