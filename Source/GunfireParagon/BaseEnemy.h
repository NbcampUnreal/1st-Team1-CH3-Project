#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class GUNFIREPARAGON_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Attack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void UseSkill();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndSkill();

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackEndDelegate OnAttackEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

protected:
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void PerformMeleeAttack() PURE_VIRTUAL(ABaseEnemy::PerformMeleeAttack, );
	virtual void PerformRangeAttack() PURE_VIRTUAL(ABaseEnemy::PerformRangeAttack, );


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")	
	bool bIsUsingSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth;
	
	void OnDeath();

};
