#include "AI/NormalRangeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"

ANormalRangeEnemy::ANormalRangeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 1000.0f;
    AttackDelay = 0.07f;
    MaxHealth = 150.0f;
    MaxWalkSpeed = 400.0f;
}

void ANormalRangeEnemy::Attack()
{
    if (!bIsAttacking && !bIsDead)
	{
        StartAttack();
		PerformRangeAttack();
	}
}

void ANormalRangeEnemy::PerformMeleeAttack()
{
	return;
}

void ANormalRangeEnemy::PerformRangeAttack()
{
    if (!GetMesh() || bIsDead) return;

    FVector Start = GetMesh()->GetSocketLocation("Muzzle_Front");
    FVector ForwardVector = GetActorForwardVector(); 
    FVector End = Start + (ForwardVector * AttackRange); 

    float CapsuleRadius = 5.0f;
    float CapsuleHalfHeight = AttackRange * 0.5f;  

    FVector CapsuleCenter = Start + (ForwardVector * CapsuleHalfHeight); // 캡슐을 총구에서 시작하도록 설정
    FQuat CapsuleRotation = FQuat::FindBetweenVectors(FVector::UpVector, ForwardVector);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    bool bHit = GetWorld()->SweepSingleByChannel
    (
        HitResult, 
        CapsuleCenter, 
        CapsuleCenter + (ForwardVector * AttackRange),
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

    DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Blue, false, 1.0f, 0, 2.0f);
}
