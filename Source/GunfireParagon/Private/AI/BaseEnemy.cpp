#include "AI/BaseEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "BehaviorTree/BehaviorTreeComponent.h" 
#include "BehaviorTree/BTTaskNode.h"  
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	Damage = 20.0f;
	AttackRange = 200.0f;
	AttackDelay = 2.0f;
	MaxHealth = 100.0f;
	BaseWalkSpeed = 300.0f;
	CurrentHealth = MaxHealth;

	bIsAttacking = false;
	bIsUsingSkill = false;
	bIsDead = false;

	Tags.Add("Monster");

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABaseEnemy::Attack(const FVector& TargetLocation)
{
}

void ABaseEnemy::StartAttack()
{
	if (bIsAttacking) return;
	bIsAttacking = true;
}

void ABaseEnemy::EndAttack()
{
	if (!bIsAttacking) return;
	bIsAttacking = false;

	AlreadyHitActors.Empty();
	UE_LOG(LogTemp, Warning, TEXT("EndAttack Called: %s"), *GetName());
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

void ABaseEnemy::ResetEnemy()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionProfileName(TEXT("Pawn"));  
		MeshComp->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);


	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->Possess(this);
		if (AIController->BrainComponent)
		{
			AIController->BrainComponent->RestartLogic(); 
		}
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	CurrentHealth = MaxHealth;
	bIsAttacking = false;
	bIsUsingSkill = false;
	bIsDead = false;
}

void ABaseEnemy::UpdateAimPitch()
{
	if (!Controller) return;

	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	FVector Start = GetMesh()->GetSocketLocation("Muzzle_Front");
	FVector Target;
	USkeletalMeshComponent* PlayerMesh = Cast<USkeletalMeshComponent>(Player->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (PlayerMesh)
	{
		Target = PlayerMesh->GetSocketLocation("upperarm_l");
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

	AimPitch = FMath::Clamp(LookAtRotation.Pitch, -60.0f, 60.0f);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("AimPitch: %f"), AimPitch));
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay - ABaseEnemy: MaxWalkSpeed = %f"), GetCharacterMovement()->MaxWalkSpeed);
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

	if (const FPointDamageEvent* PointDamageEvent = (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID) ? 
		static_cast<const FPointDamageEvent*>(&DamageEvent) : nullptr)
	{
		FName BoneName = PointDamageEvent->HitInfo.BoneName;
		if (BoneName == "head")
		{
			ActualDamage *= 2.0f;
		}

		if (DamageCauser && GetController())
		{
			UAISense_Damage::ReportDamageEvent
			(
				GetWorld(),
				this,
				DamageCauser,
				ActualDamage,
				GetActorLocation(),
				PointDamageEvent->HitInfo.ImpactPoint
			);
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
	if (!bIsDead)
	{
		bIsDead = true;
	}

	SetActorTickEnabled(false);
	SetDeathState();
	
	// GameMode 쪽 메서드로 사망처리 구현 필요
}

void ABaseEnemy::SetDeathState()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// AI 로직 정지 및 컨트롤러 해제
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->StopLogic(TEXT("Dead"));
		AIController->UnPossess();
	}

	// 충돌 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 물리 충돌 프로파일 설정
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	// 물리 속도 초기화 
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->SetSimulatePhysics(true);

	// 충격 적용 
	FVector ImpulseDirection = GetActorRotation().Vector() * -1.0f;
	ImpulseDirection.Normalize();

	float ImpulseStrength = 20.0f;  
	FVector FinalImpulse = ImpulseDirection * ImpulseStrength;

	MeshComp->AddImpulseToAllBodiesBelow(FinalImpulse);
}


