#include "AI/BTD_SetRandomLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTD_SetRandomLocation::UBTD_SetRandomLocation()
{
	NodeName = TEXT("SetRandomLocation");
}

bool UBTD_SetRandomLocation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return false;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return false;

	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp) return false;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	if (!NavSystem) return false;
	
	FVector PivotLocation = BBComp->GetValueAsVector(GetSelectedBlackboardKey());
	FNavLocation RandomLocation;
	if (NavSystem->GetRandomReachablePointInRadius(PivotLocation, PatrolRadius, RandomLocation))
	{
		BBComp->SetValueAsVector("RandomLocation", RandomLocation.Location);

		return true;
	}

	return false;
}
