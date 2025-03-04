#include "Actor/Trap/ArrowTrap.h"
#include "Components//SkeletalMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AArrowTrap::AArrowTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	ArrowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(RootComponent);
	ArrowMesh->OnComponentBeginOverlap.AddDynamic(this, &AArrowTrap::OverlapTrapMesh);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(LightIntensity);
	SpotLight->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f));
	SpotLight->SetAttenuationRadius(LightDistance); 
	SpotLight->SetOuterConeAngle(LightAngle); 
	SpotLight->SetRelativeLocation(FVector(0.0f, 0.0f, LightHeight)); 
	SpotLight->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

void AArrowTrap::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = GetActorLocation();
	RunningTime = 0.0f;
}

void AArrowTrap::ActivateTrap()
{
	UE_LOG(LogTemp, Warning, TEXT("ArrowTrap 활성화됨!"));
}

void AArrowTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;
	MoveTowardsPlayer(DeltaTime);
}

void AArrowTrap::MoveTowardsPlayer(float DeltaTime)
{
	AActor* Player = FindPlayer();
	if (!Player) return;

	FVector CurrentLocation = GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();

	FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();

	Direction.Z = 0;

	FVector TargetLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 5.0f);  
	SetActorLocation(NewLocation);

	FRotator LookAtRotation = Direction.Rotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, 3.0f);
	SetActorRotation(NewRotation);
}

AActor* AArrowTrap::FindPlayer()
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); 
}

void AArrowTrap::OverlapTrapMesh(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						 bool bFromSweep, const FHitResult& SweepResult)

{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("상어가 플레이어와 충돌!"));
		UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, GetVelocity(), SweepResult, nullptr, this, UDamageType::StaticClass());

		Destroy();
	}
}