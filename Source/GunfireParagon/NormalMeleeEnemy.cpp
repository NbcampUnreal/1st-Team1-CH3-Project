#include "NormalMeleeEnemy.h"
#include "BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"

ANormalMeleeEnemy::ANormalMeleeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 200.0f;
}

void ANormalMeleeEnemy::Attack()
{
	Super::Attack();

	if (!bIsAttacking)
	{
		PerformMeleeAttack();
	}
}

void ANormalMeleeEnemy::PerformMeleeAttack()
{
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * 200.0f);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, QueryParams);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
        }
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
}

void ANormalMeleeEnemy::PerformRangeAttack()
{
	return;
}
