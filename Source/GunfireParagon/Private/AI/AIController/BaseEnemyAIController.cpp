#include "AI/AIController/BaseEnemyAIController.h"
#include "AI/Enemy/BaseEnemy.h"
#include "Player/PlayerCharacter.h"
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
	SightConfig->SightRadius = 8000.0f; // AI의 감지 범위
	SightConfig->LoseSightRadius = 8500.0f; // AI가 플레이어를 잃어버리는 거리
	SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각 설정
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
			BBComp->SetValueAsFloat("AttackDelay", ControlledEnemy->AttackDelay);

			float AttackRange = ControlledEnemy->AttackRange;
			BBComp->SetValueAsFloat("AttackRange", AttackRange);
			BBComp->SetValueAsFloat("AcceptableRadius", AttackRange - 50.0f);

			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
			if (PlayerPawn)
			{
				if (BBComp)
				{
					BBComp->SetValueAsObject("TargetPlayer", PlayerPawn);
				}
			}
		}
	}
}

void ABaseEnemyAIController::BeginPlay()
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

void ABaseEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCharacter* TargetPlayer = Cast<APlayerCharacter>(BBComp->GetValueAsObject("TargetPlayer"));

	if (TargetPlayer && TargetPlayer->IsPlayerDead())
	{
		BBComp->ClearValue("TargetPlayer");
		BBComp->ClearValue("PlayerLocation");
		BBComp->SetValueAsBool("HasSpottedPlayer", false);
		return;
	}

	if (TargetPlayer && BBComp->GetValueAsBool("HasSpottedPlayer"))
	{
		FVector PlayerLocation = TargetPlayer->GetActorLocation();
		BBComp->SetValueAsVector("PlayerLocation", PlayerLocation);
		BBComp->ClearValue("LastKnownPlayerLocation");
	}
}

// AI가 감지한 객체의 정보를 처리하는 함수
void ABaseEnemyAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BBComp) return;
	if (!Actor || !Actor->ActorHasTag("Player")) return; 

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (!Enemy) return;

	// 플레이어가 시야에 들어온 경우
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
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
	// 플레이어가 데미지를 준 경우
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		BBComp->SetValueAsBool("HasSpottedPlayer", true); 
	}
}
