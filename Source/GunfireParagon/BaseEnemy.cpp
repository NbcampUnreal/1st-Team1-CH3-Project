#include "BaseEnemy.h"
#include "BaseEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"

ABaseEnemy::ABaseEnemy()
{
	AIControllerClass = ABaseEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	}

	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Damage = 20.0f;
	AttackRange = 200.0f;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	bIsAttacking = false;
	bIsUsingSkill = false;
}

void ABaseEnemy::Attack()
{
	if (!bIsAttacking)
	{
		StartAttack();
	}
}

void ABaseEnemy::StartAttack()
{
	bIsAttacking = true;
}

void ABaseEnemy::EndAttack()
{
	bIsAttacking = false;

	OnAttackEnd.Broadcast();
	OnAttackEnd.Clear();
}

void ABaseEnemy::UseSkill()
{
	if (!bIsUsingSkill)
	{
		bIsUsingSkill = true;
	}
}

void ABaseEnemy::EndSkill()
{
	bIsUsingSkill = false;
}

float ABaseEnemy::TakeDamage
(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		const FPointDamageEvent* PointDamageEvent = reinterpret_cast<const FPointDamageEvent*>(&DamageEvent);
		FName BoneName = PointDamageEvent->HitInfo.BoneName;
		if (BoneName == "head")
		{
			ActualDamage *= 2.0f;
		}
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ABaseEnemy::OnDeath()
{
	// 사망 로직 구현
}

