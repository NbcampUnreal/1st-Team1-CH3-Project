#include "AI/BaseMagicProjectile.h"
#include "AI/BossEnemy.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseMagicProjectile::ABaseMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionObjectType(ECC_PhysicsBody);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); 
	SetRootComponent(SphereComp);

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(SphereComp);
	ProjectileParticle->bAutoActivate = true;

	Speed = 1500.0f;
	DamageMultiplier = 1.0f;
	InitialLocation = FVector::ZeroVector;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	SphereComp->OnComponentHit.AddDynamic(this, &ABaseMagicProjectile::OnHit);
}

void ABaseMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	GetWorldTimerManager().SetTimer(DestroyCheckTimer, this, &ABaseMagicProjectile::CheckDestroyCondition, 0.2f, true);
}

void ABaseMagicProjectile::SetVelocity(FVector Direction)
{
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void ABaseMagicProjectile::CheckDestroyCondition()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	ABossEnemy* Boss = Cast<ABossEnemy>(OwnerActor);
	if (!Boss) return;

	float BossAttackRange = Boss->AttackRange;
	if (FVector::Dist(InitialLocation, GetActorLocation()) >= BossAttackRange)
	{
		Destroy();

		if (DisappearEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DisappearEffect, GetActorLocation());
		}

		GetWorld()->GetTimerManager().ClearTimer(DestroyCheckTimer);
	}
}

void ABaseMagicProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor || OwnerActor == OtherActor || OtherActor == this) return;

		ABossEnemy* Boss = Cast<ABossEnemy>(OwnerActor);
		if (!Boss) return;

		float FinalDamage = Boss->Damage * DamageMultiplier;
		UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Green, FString::Printf(TEXT("Boss Attack Hit Damage %f"), FinalDamage));
	}

	if (HitImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitImpactEffect, Hit.ImpactPoint);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
	}

	Destroy();
}