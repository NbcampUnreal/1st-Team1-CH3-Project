#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "NormalRangeEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ANormalRangeEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ANormalRangeEnemy();

	virtual void Attack(const FVector& TargetLocation) override;
	virtual void PerformRangeAttack(const FVector& TargetLocation) override;

protected:
	virtual void PerformMeleeAttack(const FVector& TargetLocation) override;

	void PlayAttackEffect(const FVector& Start, const FVector& Direction);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* RangeAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFlashEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* BulletEffect;
};
