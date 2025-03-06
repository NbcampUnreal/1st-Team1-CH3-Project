#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/BaseEnemy.h"
#include "EliteRangeEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API AEliteRangeEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	AEliteRangeEnemy();

	virtual void Attack(const FVector& TargetLocation) override;
	virtual void PerformRangeAttack(const FVector& TargetLocation) override;

protected:
	virtual void PerformMeleeAttack(const FVector& TargetLocation) override;

	void PlayAttackEffect(const FVector& Start, const FVector& Direction);

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<class AEliteGunProjectile> GunProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* RangeAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFlashEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* BulletEffect;
};
