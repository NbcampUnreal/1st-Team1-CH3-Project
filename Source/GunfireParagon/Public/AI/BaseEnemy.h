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

	virtual void Attack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void UseSkill();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndSkill();

	UFUNCTION(BlueprintCallable, Category = "State")
	void ResetEnemy();

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
	float MaxWalkSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsUsingSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead;


protected:
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void BeginPlay() override;

	virtual void PerformMeleeAttack() PURE_VIRTUAL(ABaseEnemy::PerformMeleeAttack, );
	virtual void PerformRangeAttack() PURE_VIRTUAL(ABaseEnemy::PerformRangeAttack, );

	void OnDeath();
	virtual void SetDeathState();

	TArray<AActor*> AlreadyHitActors;





};
