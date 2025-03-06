#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

UCLASS()
class GUNFIREPARAGON_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

	virtual void Tick(float DeltaTime) override;
protected:
	// AI가 Pawn을 소유할 때 호출되고 BeginPlay보다 먼저 호출된다.
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBlackboardData* BlackboardData;

	// AI Perception Component: AI가 환경을 감지하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComponent;

	// SightConfig: 시각 인식을 위한 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBlackboardComponent* BBComp;

	// Actor -> 감지된 액터, Stimulus -> 감지된 자극(시야, 소리 등)
	UFUNCTION()
	virtual void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);
};