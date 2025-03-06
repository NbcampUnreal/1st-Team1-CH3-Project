#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/BaseEnemy.h"
#include "BossEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ABossEnemy : public ABaseEnemy
{
	GENERATED_BODY()
public:
	ABossEnemy();

	virtual void Attack(const FVector& TargetLocation) override;
	virtual void PerformRangeAttack(const FVector& TargetLocation) override;

	void UseBarrageSkill();
	void UseMeteorSkill();
	void SpawnMeteor();
	void UseTeleportSkill(const FVector& TeleportPoint);
	void ActivateTeleport();
	void StartTeleportEffect();
	void UseHealSkill();
	void ActivateHeal();

	FVector GetPlayerLocation() { return LastKnownPlayerLocation; }

protected:
	virtual void PerformMeleeAttack(const FVector& TargetLocation) override;
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void FinishTeleport(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireballMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* BarrageSkillMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* MeteorSkillMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TeleportSkillMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HealSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MeteorWarningEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MeteorSpawnEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* TeleportTrailEffect;
	UPROPERTY()
	UParticleSystemComponent* TeleportTrailParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* TeleportBodyEffect;
	UPROPERTY()
	UParticleSystemComponent* TeleportBodyParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<class AFireballProjectile> FireballClass;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<class AMeteorProjectile> MeteorClass;

	FVector LastKnownPlayerLocation;
	FVector TeleportLocation;
	bool bUsedHealSkill;
};
