#pragma once

#include "CoreMinimal.h"
#include "AI/AIController/BaseEnemyAIController.h"
#include "BossAIController.generated.h"

UCLASS()
class GUNFIREPARAGON_API ABossAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	ABossAIController();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetPlayer() const;
	
	void UpdateBlackboardValue(FName KeyName, bool Value);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void DisableAI();
	UFUNCTION(BlueprintCallable, Category = "AI")
	void EnableAI();
};
