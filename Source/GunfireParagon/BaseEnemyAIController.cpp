#include "BaseEnemyAIController.h"
#include "BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

// AI 컨트롤러의 기본 생성자
ABaseEnemyAIController::ABaseEnemyAIController()
{
	// AI Perception 설정
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);

	// 시각 감지 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 3000.0f; // AI의 감지 범위
	SightConfig->LoseSightRadius = 3500.0f; // AI가 플레이어를 잃어버리는 거리
	SightConfig->PeripheralVisionAngleDegrees = 100.0f; // 시야각 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Perception 컴포넌트 설정
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::OnTargetPerceived);

	// 기본 변수 초기화
	BehaviorTree = nullptr;
	BlackboardData = nullptr;
	BlackboardComponent = nullptr;
}

// AI가 Pawn을 소유할 때 호출
void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BlackboardData)
	{
		if (UseBlackboard(BlackboardData, BlackboardComponent))
		{
			BlackboardComponent = GetBlackboardComponent();
			BlackboardComponent->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());
		}
	}

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}


void ABaseEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

// AI가 감지한 객체의 정보를 처리하는 함수
void ABaseEnemyAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BlackboardComponent) return;
	if (!Actor || !Actor->ActorHasTag("Player")) return; 

	if (Stimulus.WasSuccessfullySensed())
	{
		// 감지된 플레이어를 블랙보드에 저장하고 초점을 맞춤
		BlackboardComponent->SetValueAsObject("TargetPlayer", Actor);
		SetFocus(Actor);
	}
	else
	{
		// 플레이어가 사라지면 블랙보드에서 제거하고 초점을 해제
		BlackboardComponent->ClearValue("TargetPlayer");
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
