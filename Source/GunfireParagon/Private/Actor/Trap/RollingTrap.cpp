#include "Actor/Trap/RollingTrap.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"



ARollingTrap::ARollingTrap()
{

	PrimaryActorTick.bCanEverTick = true;

	DamageCollider = CreateDefaultSubobject<USphereComponent>(TEXT("DamageCollider"));
	DamageCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = DamageCollider;
	DamageCollider->OnComponentBeginOverlap.AddDynamic(this, &ARollingTrap::OnTrapTriggered);
	DamageCollider->OnComponentBeginOverlap.AddDynamic(this, &ARollingTrap::OnOverlapWithDestroyBox);

	BoulderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoulderMesh"));
	BoulderMesh->SetupAttachment(DamageCollider);
	BoulderMesh->SetSimulatePhysics(true); 
	BoulderMesh->SetNotifyRigidBodyCollision(true);
	BoulderMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  
	BoulderMesh->SetCollisionObjectType(ECC_PhysicsBody); 
	BoulderMesh->OnComponentHit.AddDynamic(this, &ARollingTrap::OnHit);
	
	StartLocation = GetActorLocation();

	
}

void ARollingTrap::BeginPlay()
{
	Super::BeginPlay();
    
}

void ARollingTrap::ActivateTrap()
{
	Super::ActivateTrap();
}

void ARollingTrap::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("Trap"))
		return;
	
	if (!bHasHitGround && OtherActor->ActorHasTag("Ground"))
	{
		UE_LOG(LogTemp, Warning, TEXT("바닥과 부딪힘"));
		bHasHitGround = true; 
	}
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어와 부딪힘"));
		bHasHitGround = true; 
	BoulderMesh->SetSimulatePhysics(false); 
	}
}

void ARollingTrap::OnOverlapWithDestroyBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapWithDestroyBox 호출됨!"));
	if (OtherActor && OtherActor->ActorHasTag("DestroyBox")) 
	{
		UE_LOG(LogTemp, Warning, TEXT("RollingTrap collided with DestroyBox: %s"), *OtherActor->GetName());
		SetActorLocation(StartLocation);
	}
}

void ARollingTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Pitch += RotationSpeed * DeltaTime;  // X축(Pitch) 회전
	SetActorRotation(NewRotation);
}

