#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "NormalMeleeEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ANormalMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	ANormalMeleeEnemy();

	virtual void Attack(const FVector& TargetLocation) override;
	virtual void PerformMeleeAttack(const FVector& TargetLocation) override;

protected:
	virtual void PerformRangeAttack(const FVector& TargetLocation) override;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* MeleeAttackMontage;
};
