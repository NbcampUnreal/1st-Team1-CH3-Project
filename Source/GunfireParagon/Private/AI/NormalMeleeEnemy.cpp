#include "AI/NormalMeleeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ANormalMeleeEnemy::ANormalMeleeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 200.0f;
    AttackDelay = 0.0f;
    MaxHealth = 200.0f;
    BaseWalkSpeed = 800.0f;
    EXP = 10.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    EnemyName = "MinionMelee";
}

void ANormalMeleeEnemy::Attack(const FVector& TargetLocation)
{
    if (!bIsAttacking && !bIsDead)
    {
        if (MeleeAttackMontage)
        {
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(MeleeAttackMontage);
                
                FOnMontageEnded MontageEndDelegate;
                MontageEndDelegate.BindUObject(this, &ANormalMeleeEnemy::OnAttackMontageEnded);

                AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, MeleeAttackMontage);
            }
        }
        StartAttack();
    }
}

void ANormalMeleeEnemy::PerformMeleeAttack(const FVector& TargetLocation)
{
    if (!GetMesh() || !bIsAttacking || bIsDead) return; 

    FVector Start = GetMesh()->GetSocketLocation("FX_Trail_02_R");
    FVector End = GetMesh()->GetSocketLocation("FX_Trail_01_R");   
    FVector MidPoint = (Start + End) * 0.5f;
    FVector SwordDirection = (End - Start).GetSafeNormal();

    FQuat SwordRotation = FQuat::FindBetweenVectors(FVector::UpVector, SwordDirection);

    float CapsuleRadius = 15.0f; 
    float CapsuleHalfHeight = (Start - End).Size() * 0.6f;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel
    (
        HitResult, 
        MidPoint,
        MidPoint, 
        SwordRotation,
        ECC_Pawn,
        FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
        QueryParams
    );

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->ActorHasTag("Player"))
        {
            if (!AlreadyHitActors.Contains(HitActor))
            {
                AlreadyHitActors.Add(HitActor);
                UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
            }
            
        }
    }
}

void ANormalMeleeEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
	return;
}
