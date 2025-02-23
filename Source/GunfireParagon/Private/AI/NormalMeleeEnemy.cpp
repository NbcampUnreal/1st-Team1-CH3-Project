#include "AI/NormalMeleeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ANormalMeleeEnemy::ANormalMeleeEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 20.0f;
    AttackRange = 200.0f;
    AttackDelay = 0.0f;
    MaxHealth = 200.0f;
    BaseWalkSpeed = 900.0f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void ANormalMeleeEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsAttacking)
    {
        PerformMeleeAttack(LastKnownPlayerLocation);
    }
}

void ANormalMeleeEnemy::Attack(const FVector& TargetLocation)
{
    if (!bIsAttacking && !bIsDead)
	{
        StartAttack();

        LastKnownPlayerLocation = TargetLocation;
	}
}

void ANormalMeleeEnemy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay - NormalMeleeEnemy: MaxWalkSpeed = %f"), GetCharacterMovement()->MaxWalkSpeed);
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
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Melee Hit: %s"), *HitActor->GetName()));
                UGameplayStatics::ApplyDamage(HitActor, Damage, GetController(), this, UDamageType::StaticClass());
            }
            
        }
    }

    DrawDebugCapsule(GetWorld(), MidPoint, CapsuleHalfHeight, CapsuleRadius, SwordRotation, FColor::Red, false, 0.5f, 0, 2.0f);
}

void ANormalMeleeEnemy::PerformRangeAttack(const FVector& TargetLocation)
{
	return;
}
