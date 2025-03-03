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
#include "GameMode/FPSGameState.h"
#include "GameMode/AIObjectPool.h"
#include "GameMode/FPSGameInstance.h"


ABaseEnemy::ABaseEnemy()
{
	AIControllerClass = ABaseEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		//SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		//SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
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

	EnemyName = "BaseMonster";
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	//OnTargetName.Broadcast(GetClass()->GetName());
	OnTargetName.Broadcast(EnemyName);
	OnTargetHPChanged.Broadcast(CurrentHealth, MaxHealth);
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

void ABaseEnemy::ResetEnemy()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		MeshComp->SetSimulatePhysics(false);
		MeshComp->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

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
		Target = PlayerMesh->GetSocketLocation("upperarm_r");
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

	AimPitch = FMath::Clamp(LookAtRotation.Pitch, -60.0f, 60.0f);
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

	if (ActualDamage > 0.0f && !bIsDead && HitReactionMontage)
	{
		PlayHitReaction();
	}

	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}

	OnTargetHPChanged.Broadcast(CurrentHealth, MaxHealth);

	return ActualDamage;
}

void ABaseEnemy::PlayHitReaction()
{
	if (!GetMesh() || !HitReactionMontage) return;
	if (bIsAttacking) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(HitReactionMontage)) return;

	AnimInstance->Montage_Play(HitReactionMontage, 2.0f);

}

void ABaseEnemy::OnDeath()
{
	if (!bIsDead)
	{
		bIsDead = true;
	}

	SetActorTickEnabled(false);
	SetDeathState();

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
	
	AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (FPSGameState)
	{
		FPSGameState->OnEnemyKilled();
	}

	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (FPSGameInstance)
	{
		FPSGameInstance->AddExperiencePoint(EXP);
	}

	FTimerHandle InvisibleTimer;
	GetWorldTimerManager().SetTimer(InvisibleTimer, this, &ABaseEnemy::ReturnToPool, 3.0f, false);
}

void ABaseEnemy::ReturnToPool()
{
	TArray<AActor*> FoundAIPool;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIObjectPool::StaticClass(), FoundAIPool);
	if (FoundAIPool.Num() == 0) return;
	else
	{
		AAIObjectPool* AIobjectPool = Cast<AAIObjectPool>(FoundAIPool[0]);
		if (AIobjectPool)
		{
			AIobjectPool->ReturnAIToPool(this);
		}
	}
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
	MeshComp->SetCollisionResponseToChannel(ECC_EngineTraceChannel2, ECR_Ignore);

	// 물리 속도 초기화 
	MeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	MeshComp->SetSimulatePhysics(true);

	//// 충격 적용 
	//FVector ImpulseDirection = GetActorRotation().Vector() * -1.0f;
	//ImpulseDirection.Normalize();

	//float ImpulseStrength = 20.0f;  
	//FVector FinalImpulse = ImpulseDirection * ImpulseStrength;

	//MeshComp->AddImpulseToAllBodiesBelow(FinalImpulse);
}

void ABaseEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EndAttack();

	if (OnSkillMontageEnded.IsBound())
	{
		OnSkillMontageEnded.Broadcast();
		OnSkillMontageEnded.Clear();
	}
}


