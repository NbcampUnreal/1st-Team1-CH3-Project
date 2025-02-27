#include "AI/BaseEnemyAIController.h"
#include "AI/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"


ABaseEnemyAIController::ABaseEnemyAIController()
{
	// AI Perception 설정
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);

	// 시각 감지 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 5000.0f; // AI의 감지 범위
	SightConfig->LoseSightRadius = 5500.0f; // AI가 플레이어를 잃어버리는 거리
	SightConfig->PeripheralVisionAngleDegrees = 160.0f; // 시야각 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 피격 감지 설정
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(5.0f);
	
	// Perception 컴포넌트 설정
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*DamageConfig);
	PerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	PerceptionComponent->RequestStimuliListenerUpdate();
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::OnTargetPerceived);

	// 기본 변수 초기화
	BehaviorTree = nullptr;
	BlackboardData = nullptr;
	BBComp = nullptr;
}

// AI가 Pawn을 소유할 때 호출
void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}

	if (BlackboardData)
	{
		ABaseEnemy* ControlledEnemy = Cast<ABaseEnemy>(InPawn);
		if (!ControlledEnemy) return;

		if (UseBlackboard(BlackboardData, BBComp))
		{
			BBComp = GetBlackboardComponent();
			BBComp->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());
			BBComp->SetValueAsFloat("AttackRange", ControlledEnemy->AttackRange - 100.0f);
			BBComp->SetValueAsFloat("AttackDelay", ControlledEnemy->AttackDelay);
		}
	}
}


void ABaseEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

// AI가 감지한 객체의 정보를 처리하는 함수
void ABaseEnemyAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BBComp) return;
	if (!Actor || !Actor->ActorHasTag("Player")) return; 

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (!Enemy) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		BBComp->SetValueAsObject("TargetPlayer", Actor);
		BBComp->SetValueAsBool("HasSpottedPlayer", true);
		if (!BBComp->GetValueAsBool("PauseLastKnownUpdate"))
		{
			BBComp->SetValueAsVector("LastKnownPlayerLocation", Actor->GetActorLocation());
		}
	}
	else
	{
		if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
		{
			if (Stimulus.GetAge() < 5.0f)
			{
				return;
			}
		}

		BBComp->ClearValue("TargetPlayer");
		BBComp->SetValueAsBool("HasSpottedPlayer", false);
	}
}
