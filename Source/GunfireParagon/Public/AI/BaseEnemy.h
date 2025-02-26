#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Attack(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "State")
	void ResetEnemy();

	void UpdateAimPitch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseWalkSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsUsingSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float AimPitch;
	

protected:
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void BeginPlay() override;

	virtual void PerformMeleeAttack(const FVector& TargetLocation) PURE_VIRTUAL(ABaseEnemy::PerformMeleeAttack, );
	virtual void PerformRangeAttack(const FVector& TargetLocation) PURE_VIRTUAL(ABaseEnemy::PerformRangeAttack, );

	void OnDeath();
	virtual void SetDeathState();

	TArray<AActor*> AlreadyHitActors;

};
