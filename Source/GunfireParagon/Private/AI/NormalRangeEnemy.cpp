#include "AI/NormalRangeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ANormalRangeEnemy::ANormalRangeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 900.0f;
    AttackDelay = 1.0f;
    MaxHealth = 150.0f;
    BaseWalkSpeed = 700.0f;
    EXP = 10.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    EnemyName = "MinionRange";
}

void ANormalRangeEnemy::Attack(const FVector& TargetLocation)
{
    if (!bIsAttacking && !bIsDead)
	{
        if (RangeAttackMontage)
        {
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(RangeAttackMontage);
              
                FOnMontageEnded MontageEndDelegate;
                MontageEndDelegate.BindUObject(this, &ANormalRangeEnemy::OnAttackMontageEnded);

                AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, RangeAttackMontage);
            }
        }
        StartAttack();
	}
}

void ANormalRangeEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
    if (!GetMesh() || !bIsAttacking || bIsDead) return;

    UpdateAimPitch(); 

    FVector Start = GetMesh()->GetSocketLocation("Muzzle_Front");
    FVector ToTarget = (TargetLocation - Start).GetSafeNormal();
    FVector ForwardDirection = GetActorForwardVector();

    FRotator AdjustedRotation = ForwardDirection.Rotation();
    AdjustedRotation.Pitch += AimPitch;
    AdjustedRotation.Yaw -= 2.0f;

    FVector AdjustedDirection = AdjustedRotation.Vector();
    AdjustedDirection.Normalize();

    float CapsuleRadius = 9.0f;
    float CapsuleHalfHeight = AttackRange * 0.5f;

    FVector CapsuleCenter = Start + (AdjustedDirection * CapsuleHalfHeight);
    FQuat CapsuleRotation = FQuat::FindBetweenVectors(FVector::UpVector, AdjustedDirection);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        CapsuleCenter,
        CapsuleCenter + (AdjustedDirection * AttackRange),
        CapsuleRotation,
        ECC_Pawn,
        FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
        QueryParams
    );

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->ActorHasTag("Player"))
        {
            GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Blue, FString::Printf(TEXT("Range Monster Attack Hit Damage %f"), Damage));
            UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
        }
    }

    PlayAttackEffect(Start, AdjustedDirection);
}

void ANormalRangeEnemy::PlayAttackEffect(const FVector& Start, const FVector& Direction)
{
    if (MuzzleFlashEffect)
    {
        FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_Front");
        FRotator MuzzleRotation = GetMesh()->GetSocketRotation("Muzzle_Front");

        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            MuzzleFlashEffect,
            MuzzleLocation,
            MuzzleRotation
        );
    }

    if (BulletEffect)
    {
        FRotator BulletRotation = Direction.Rotation();

        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            BulletEffect,
            Start,
            BulletRotation
        );
    }
}

void ANormalRangeEnemy::PerformMeleeAttack(const FVector& TargetLocation)
{
    return;
}
