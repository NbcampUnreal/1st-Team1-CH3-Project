#include "AI/BossAIController.h"
#include "AI/BossEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ABossAIController::ABossAIController()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 7000.0f; // AI의 감지 범위
		SightConfig->LoseSightRadius = 8000.0f; // AI가 플레이어를 잃어버리는 거리
		SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각 설정
	}
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BBComp)
	{
		BBComp->RemoveFromRoot();
		BBComp = nullptr;
	}

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (!Enemy) return;

	BBComp = NewObject<UBlackboardComponent>(this, UBlackboardComponent::StaticClass());
	if (UseBlackboard(BlackboardData, BBComp))
	{
		BBComp->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());
		BBComp->SetValueAsFloat("AttackRange", Enemy->AttackRange);
	}
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		if (BBComp)
		{
			BBComp->SetValueAsObject("TargetPlayer", PlayerPawn);
		}
	}
}

void ABossAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!BBComp) return;

	AActor* TargetPlayer = Cast<AActor>(BBComp->GetValueAsObject("TargetPlayer"));
	if (TargetPlayer && BBComp->GetValueAsBool("HasSpottedPlayer")) 
	{
		FVector PlayerLocation = TargetPlayer->GetActorLocation();
		BBComp->SetValueAsVector("PlayerLocation", PlayerLocation);
		BBComp->ClearValue("LastKnownPlayerLocation");
	}
}

void ABossAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BBComp) return;
	if (!Actor || !Actor->ActorHasTag("Player")) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(GetPawn());
	if (!Boss) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		BBComp->SetValueAsBool("HasSpottedPlayer", true);
	}
	else
	{
		FVector PlayerLocation = BBComp->GetValueAsVector("PlayerLocation");
		BBComp->SetValueAsVector("LastKnownPlayerLocation", PlayerLocation);
		BBComp->ClearValue("PlayerLocation");
		BBComp->SetValueAsBool("HasSpottedPlayer", false);
	}
}

AActor* ABossAIController::GetTargetPlayer() const
{
	if (BBComp)
	{
		return Cast<AActor>(BBComp->GetValueAsObject("TargetPlayer"));
	}

	return nullptr;
}

void ABossAIController::UpdateBlackboardValue(FName KeyName, bool Value)
{
	if (!BBComp) return;

	BBComp->SetValueAsBool(KeyName, Value);
}