#include "AI/Enemy/BossEnemy.h"
#include "AI/AIController/BossAIController.h"
#include "AI/Projectile/FireballProjectile.h"
#include "AI/Projectile/MeteorProjectile.h"
#include "AIController.h"
#include "GameMode/FPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ABossEnemy::ABossEnemy()
{
    AIControllerClass = ABossAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 3000.0f;
    AttackDelay = 1.0f;
    MaxHealth = 5000.0f;
    BaseWalkSpeed = 400.0f;
    CurrentHealth = MaxHealth;
    LastKnownPlayerLocation = FVector::ZeroVector;
    TeleportLocation = FVector::ZeroVector;
    EXP = 0.0f;
    bUsedHealSkill = false;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    EnemyName = "Boss";
}

void ABossEnemy::Attack(const FVector& TargetLocation)
{
    if (!bIsAttacking && !bIsDead)
    {
        StartAttack();

        LastKnownPlayerLocation = TargetLocation;

        if (FireballMontage)
        {
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            if (AnimInstance)
            {
                AnimInstance->Montage_Play(FireballMontage);

                FOnMontageEnded MontageEndDelegate;
                MontageEndDelegate.BindUObject(this, &ABossEnemy::OnAttackMontageEnded);

                AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, FireballMontage);
            }
        }
    }
}

void ABossEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
    if (!FireballClass) return;

    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp) return;

    FVector HandLocation = MeshComp->GetSocketLocation("hand_rSocket");
    FVector SpawnLocation = HandLocation + GetActorForwardVector() * 150.0f;

    AActor* TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!TargetActor) return;

    FVector PlayerVelocity = TargetActor->GetVelocity(); 
    FVector PlayerLocation = TargetActor->GetActorLocation();

    // 예측 시간
    float PredictionTime = 0.3f; 
    // 현재 위치에 플레이어의 속도 * 시간으로 거리를 더해 예측 위치 계산
    FVector PredictedLocation = PlayerLocation + (PlayerVelocity * PredictionTime);
    // 상체 조준하도록 Z축 조정
    PredictedLocation.Z += 60.0f; 

    FRotator FireRotation = (PredictedLocation - SpawnLocation).Rotation();
    AFireballProjectile* Fireball = GetWorld()->SpawnActor<AFireballProjectile>(FireballClass, SpawnLocation, FireRotation);
    if (Fireball)
    {
        Fireball->SetOwner(this);
        Fireball->SetVelocity(FireRotation.Vector());
    }
}

void ABossEnemy::UseBarrageSkill()
{
    if (BarrageSkillMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(BarrageSkillMontage);

            FOnMontageEnded MontageEndDelegate;
            MontageEndDelegate.BindUObject(this, &ABossEnemy::OnAttackMontageEnded);

            AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, BarrageSkillMontage);
        }
        StartAttack();
    }
}

void ABossEnemy::UseMeteorSkill()
{
    if (MeteorSkillMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(MeteorSkillMontage);
            FOnMontageEnded MontageEndDelegate;
            MontageEndDelegate.BindUObject(this, &ABossEnemy::OnAttackMontageEnded);
            AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, MeteorSkillMontage);
        }
        StartAttack();
    }
}

void ABossEnemy::SpawnMeteor()
{
    if (!MeteorClass) return;

    AActor* TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!TargetActor) return;

    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector PlayerVelocity = TargetActor->GetVelocity();


    float PredictionTime = 0.6f;
    FVector PredictedLocation = PlayerLocation + (PlayerVelocity * PredictionTime);
    FVector SpawnLocation = PredictedLocation + FVector(-100.0f, 0.0f, 3000.0f);

    FRotator MeteorRotation = FRotator(-90.0f, 0.0f, 0.0f);

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorSpawnEffect, SpawnLocation, FRotator::ZeroRotator, true);
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorWarningEffect, SpawnLocation, FRotator::ZeroRotator, true);

    AMeteorProjectile* Meteor = GetWorld()->SpawnActor<AMeteorProjectile>(MeteorClass, SpawnLocation, MeteorRotation);
    if (Meteor)
    {
        Meteor->SetOwner(this);
        Meteor->SetVelocity(MeteorRotation.Vector());
    }
}

void ABossEnemy::UseTeleportSkill(const FVector& TeleportPoint)
{
    if(TeleportSkillMontage)
    {
        TeleportLocation = TeleportPoint;

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(TeleportSkillMontage);
            FOnMontageEnded MontageEndDelegate;
            MontageEndDelegate.BindUObject(this, &ABossEnemy::FinishTeleport);
            AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, TeleportSkillMontage);
        }
        StartAttack();
    }  
}

void ABossEnemy::ActivateTeleport()
{
    if (TeleportLocation == FVector::ZeroVector) return;
    
    TeleportLocation.Z += 50.0f;

    StartTeleportEffect();

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        GetCharacterMovement()->SetMovementMode(MOVE_Flying);
        GetCharacterMovement()->MaxFlySpeed = 8000.0f;
        GetCharacterMovement()->MaxAcceleration = 99999.0f;

        AIController->MoveToLocation(TeleportLocation, 10.0f, false, false, false, false, 0, false);
    }
}

void ABossEnemy::StartTeleportEffect()
{
    if (TeleportBodyEffect)
    {
        TeleportBodyParticle = UGameplayStatics::SpawnEmitterAttached(TeleportBodyEffect, GetMesh(), TEXT("Spine_01"));
    }

    if (TeleportTrailEffect)
    {
        TeleportTrailParticle = UGameplayStatics::SpawnEmitterAttached(TeleportTrailEffect, GetMesh(), TEXT("Spine_01"));
    }

    GetMesh()->SetVisibility(false);
}

void ABossEnemy::FinishTeleport(UAnimMontage* Montage, bool bInterrupted)
{
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    GetCharacterMovement()->MaxAcceleration = 2048.0f;
    GetMesh()->SetVisibility(true);

    TeleportLocation = FVector::ZeroVector;

    if (TeleportBodyParticle)
    {
        TeleportBodyParticle->DestroyComponent();
        TeleportBodyParticle = nullptr;
    }

    if (TeleportTrailParticle)
    {
        TeleportTrailParticle->DestroyComponent();
        TeleportTrailParticle = nullptr;
    }

    OnAttackMontageEnded(TeleportSkillMontage, false);
}

void ABossEnemy::UseHealSkill()
{
    if (HealSkillMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(HealSkillMontage);
            FOnMontageEnded MontageEndDelegate;
            MontageEndDelegate.BindUObject(this, &ABossEnemy::FinishTeleport);
            AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, HealSkillMontage);
        }
        StartAttack();
    }
}

void ABossEnemy::ActivateHeal()
{
    float LostHealth = MaxHealth - CurrentHealth;
    float HealAmount = LostHealth * 0.5f;

    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
}

float ABossEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (CurrentHealth <= MaxHealth * 0.5f && !bUsedHealSkill)
    {
        ABossAIController* BossController = Cast<ABossAIController>(GetController());
        if (!BossController) return ActualDamage;

        BossController->UpdateBlackboardValue("ShouldHealSkill", true);
        bUsedHealSkill = true;
    }

    return ActualDamage;
}

void ABossEnemy::OnDeath()
{
    if (bIsDead) return;
    bIsDead = true;

    SetActorTickEnabled(false);
    SetDeathState();

    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

    AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (FPSGameMode)
    {
        FPSGameMode->EndGame(true);
    }
}

void ABossEnemy::PerformMeleeAttack(const FVector& TargetLocation)
{
    return;
}


