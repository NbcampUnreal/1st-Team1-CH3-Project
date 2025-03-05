#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSkillMontageEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedEnemyValue, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingEnemyName, FString, TargetName);

UCLASS()
class GUNFIREPARAGON_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();
	
	virtual void BeginPlay() override;
	virtual void Attack(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "State")
	void ResetEnemy();

	void UpdateAimPitch();
	void SetCanAttack(bool bNewState) { bCanAttack = bNewState; }

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float EXP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float AimPitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsUsingSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead;

	bool bCanAttack;
	
	FOnSkillMontageEnded OnSkillMontageEnded;

protected:
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void PerformMeleeAttack(const FVector& TargetLocation) PURE_VIRTUAL(ABaseEnemy::PerformMeleeAttack, );
	virtual void PerformRangeAttack(const FVector& TargetLocation) PURE_VIRTUAL(ABaseEnemy::PerformRangeAttack, );
	virtual void SetDeathState();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HitReactionMontage;
	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundBase* DeathSound;

	void PlayHitReaction();
	void OnDeath();
	void ReturnToPool();

	TArray<AActor*> AlreadyHitActors;

public:
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedEnemyValue OnTargetHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnSettingEnemyName OnTargetName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enemy Name")
	FString EnemyName;
};
