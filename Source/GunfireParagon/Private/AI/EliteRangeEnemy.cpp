#include "AI/EliteRangeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "AI/EliteGunProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEliteRangeEnemy::AEliteRangeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 30.0f;
    AttackRange = 1000.0f;
    AttackDelay = 4.0f;
    MaxHealth = 300.0f;
    BaseWalkSpeed = 600.0f;
    CurrentHealth = MaxHealth;

    EnemyName = "EliteRange";
}

void AEliteRangeEnemy::Attack(const FVector& TargetLocation)
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
                MontageEndDelegate.BindUObject(this, &AEliteRangeEnemy::OnAttackMontageEnded);

                AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, RangeAttackMontage);
            }
        }
        StartAttack();
    }
}

void AEliteRangeEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
    if (!GunProjectileClass) return;

    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp) return;

    FVector MuzzleLocation = MeshComp->GetSocketLocation("Muzzle_02");
    FVector SpawnLocation = MuzzleLocation + GetActorForwardVector() * 150.0f;

    AActor* TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!TargetActor) return;

    FVector PlayerVelocity = TargetActor->GetVelocity();
    FVector PlayerLocation = TargetActor->GetActorLocation();

    // 예측 시간
    float PredictionTime = 0.35f;
    // 현재 위치에 플레이어의 속도 * 시간으로 거리를 더해 예측 위치 계산
    FVector PredictedLocation = PlayerLocation + (PlayerVelocity * PredictionTime);
    // 상체 조준하도록 Z축 조정
    PredictedLocation.Z += 55.0f;

    FRotator FireRotation = (PredictedLocation - SpawnLocation).Rotation();
    AEliteGunProjectile* GunProjectile = GetWorld()->SpawnActor<AEliteGunProjectile>(GunProjectileClass, SpawnLocation, FireRotation);
    if (GunProjectile)
    {
        GunProjectile->SetOwner(this);
        GunProjectile->SetVelocity(FireRotation.Vector());
    }

    PlayAttackEffect(MuzzleLocation, FireRotation.Vector());
}

void AEliteRangeEnemy::PlayAttackEffect(const FVector& Start, const FVector& Direction)
{
    if (MuzzleFlashEffect)
    {
        FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_02");
        FRotator MuzzleRotation = GetMesh()->GetSocketRotation("Muzzle_02");

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

void AEliteRangeEnemy::PerformMeleeAttack(const FVector& TargetLocation)
{
    return;
}
