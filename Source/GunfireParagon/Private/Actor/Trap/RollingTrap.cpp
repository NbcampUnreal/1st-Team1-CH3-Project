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
}

void ARollingTrap::BeginPlay()
{
	Super::BeginPlay();
	Velocity = FVector(0.0f, 0.0f, 0.0f);
	StartLocation = GetActorLocation();
	GetWorldTimerManager().SetTimer(DropTimerHandle, this, &ARollingTrap::StartFalling, DropInterval, true);


}

void ARollingTrap::ActivateTrap()
{
	Super::ActivateTrap();
}

void ARollingTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bCanFall) return;
	
	if (!bHasHitGround)
	{
		Velocity.Z -= GravityScale * DeltaTime;
	}

	FVector MoveDirection;
	if (!bHasHitGround)
	{
		MoveDirection = GetActorForwardVector() * MoveSpeed * DeltaTime + FVector(0.0f, 0.0f, Velocity.Z);
	}
	else
	{
		MoveDirection = GetActorForwardVector() * MoveSpeed * DeltaTime;  
	}
	FHitResult HitResult;
	SetActorLocation(GetActorLocation() + MoveDirection, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Warning, TEXT("바닥과 충돌 감지됨!"));
        
		Velocity = FVector(GetActorForwardVector() * MoveSpeed); 
		bHasHitGround = true;
	}
	else
	{
		bHasHitGround = false;	
	}
	

	FRotator MeshRotation = BoulderMesh->GetRelativeRotation();
	MeshRotation.Pitch += RotationSpeed * DeltaTime;  // X축 회전 (굴러가는 효과)
	BoulderMesh->SetRelativeRotation(MeshRotation);
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
	
}

void ARollingTrap::OnOverlapWithDestroyBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor && OtherActor->ActorHasTag("DestroyBox"))
	{
	UE_LOG(LogTemp, Warning, TEXT("RollingTrap collided with DestroyBox: %s"), *OtherActor->GetName());
		SetActorLocation(StartLocation);
		Velocity = FVector(0.0f, 0.0f, 0.0f); 
	}
}

void ARollingTrap::StartFalling()
{
	if (!bCanFall)
	{
		UE_LOG(LogTemp, Warning, TEXT("RollingTrap 시작됨!"));

		SetActorLocation(StartLocation);
		Velocity = FVector(0.0f, 0.0f, 0.0f);
		bHasHitGround = false;  
        
		bCanFall = true; 
		SetActorTickEnabled(true);
	}
}
