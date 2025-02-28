#include "AI/BTService_UpdateDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/BaseEnemy.h"

UBTService_UpdateDistance::UBTService_UpdateDistance()
{
	NodeName = TEXT("Update Distance to Player");
	Interval = 0.3f;
}

void UBTService_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	AActor* TargetPlayer = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetPlayer"));	
	if (!TargetPlayer || !TargetPlayer->ActorHasTag("Player")) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (!Enemy) return;

	float Distance = FVector::Dist(Enemy->GetActorLocation(), TargetPlayer->GetActorLocation());

	BlackboardComponent->SetValueAsFloat("DistanceToPlayer", Distance);
}
