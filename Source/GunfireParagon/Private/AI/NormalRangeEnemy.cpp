#include "AI/NormalRangeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ANormalRangeEnemy::ANormalRangeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 1000.0f;
    AttackDelay = 0.06f;
    MaxHealth = 150.0f;
    BaseWalkSpeed = 600.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void ANormalRangeEnemy::Attack(const FVector& TargetLocation)
{
    if (!bIsAttacking && !bIsDead)
	{
        StartAttack();
		PerformRangeAttack(TargetLocation);
	}
}

void ANormalRangeEnemy::PerformMeleeAttack(const FVector& TargetLocation)
{
	return;
}

void ANormalRangeEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
    if (!GetMesh() || bIsDead) return;

    UpdateAimPitch(); 

    FVector Start = GetMesh()->GetSocketLocation("Muzzle_Front");
    FVector ToTarget = (TargetLocation - Start).GetSafeNormal();
    FVector ForwardDirection = GetActorForwardVector();

    FRotator AdjustedRotation = ForwardDirection.Rotation();
    AdjustedRotation.Pitch += AimPitch;

    FVector AdjustedDirection = AdjustedRotation.Vector();
    AdjustedDirection.Normalize();

    float CapsuleRadius = 5.0f;
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
    DrawDebugCapsule(GetWorld(),
        CapsuleCenter,
        CapsuleHalfHeight,
        CapsuleRadius,
        CapsuleRotation,
        bHit ? FColor::Red : FColor::Green, // 히트 여부에 따라 색상 변경
        false,
        2.0f,  // 지속 시간
        0,
        2.0f  // 두께
    );

    PlayMuzzleFlashEffect();
    PlayBulletEffect(Start, AdjustedDirection);

}

void ANormalRangeEnemy::PlayMuzzleFlashEffect()
{
    if (!MuzzleFlashEffect || !GetMesh()) return;

    FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_Front");
    FRotator MuzzleRotation = GetMesh()->GetSocketRotation("Muzzle_Front");

    UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),         
        MuzzleFlashEffect,  
        MuzzleLocation,     
        MuzzleRotation      
    );
}

void ANormalRangeEnemy::PlayBulletEffect(const FVector& Start, const FVector& Direction)
{
    if (!BulletEffect || !GetWorld()) return;

    FRotator BulletRotation = Direction.Rotation();

    UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),       
        BulletEffect,     
        Start,            
        BulletRotation    
    );
}
