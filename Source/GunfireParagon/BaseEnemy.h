#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Range UMETA(DisplayName = "Range"),
};

UCLASS()
class GUNFIREPARAGON_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Attack(EEnemyAttackType AttackType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void UseSkill();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndSkill();

protected:
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void PerformMeleeAttack() PURE_VIRTUAL(ABaseEnemy::PerformMeleeAttack, );
	virtual void PerformRangeAttack() PURE_VIRTUAL(ABaseEnemy::PerformRangeAttack, );

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<EEnemyAttackType> AttackTypes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")	
	bool bIsUsingSkill;

	float CurrentHealth;
	float MaxHealth;
	float Damage;
	
	void OnDeath();

};
