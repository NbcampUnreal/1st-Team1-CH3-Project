#include "NormalRangeEnemy.h"
#include "BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"

ANormalRangeEnemy::ANormalRangeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 1000.0f;
}

void ANormalRangeEnemy::Attack()
{
	Super::Attack();

	if (!bIsAttacking)
	{
		PerformRangeAttack();
	}
}

void ANormalRangeEnemy::PerformMeleeAttack()
{
	return;
}

void ANormalRangeEnemy::PerformRangeAttack()
{
    FVector Start = GetActorLocation() + FVector(0, 0, 50);
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * 1000.0f);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
        }
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0f, 0, 2.0f);
}
