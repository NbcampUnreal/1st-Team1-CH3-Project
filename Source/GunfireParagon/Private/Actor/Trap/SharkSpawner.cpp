// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Trap/SharkSpawner.h"
#include "Actor/Trap/ArrowTrap.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ASharkSpawner::ASharkSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	TriggerBox_Entry = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox_Entry"));
	TriggerBox_Entry->SetupAttachment(RootComponent);
	TriggerBox_Entry->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerBox_Entry->OnComponentBeginOverlap.AddDynamic(this, &ASharkSpawner::OnEntryTriggered);

	TriggerBox_Exit = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox_Exit"));
	TriggerBox_Exit->SetupAttachment(RootComponent);
	TriggerBox_Exit->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerBox_Exit->OnComponentBeginOverlap.AddDynamic(this, &ASharkSpawner::OnExitTriggered);

}

// Called when the game starts or when spawned
void ASharkSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASharkSpawner::OnEntryTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player") && !bIsActive)
	{
		bIsActive = true;
		SpawnShark();
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASharkSpawner::SpawnShark, SpawnInterval, true);
	}
}
void ASharkSpawner::OnExitTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player") && bIsActive)
	{

		bIsActive = false;
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		TArray<AActor*> Sharks;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SharkClass, Sharks);
		for (AActor* Shark : Sharks)
		{
			AArrowTrap* ArrowTrap = Cast<AArrowTrap>(Shark);
			if (ArrowTrap)
			{
				ArrowTrap->SetActorEnableCollision(false);  
				ArrowTrap->Destroy(); 
			}
		}
	}
}

void ASharkSpawner::SpawnShark()
{
	if (!bIsActive || !SharkClass) return;

	TArray<AActor*> ExistingSharks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SharkClass, ExistingSharks);

	if (ExistingSharks.Num() >= 10)
	{
		return;  
	}


	FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AArrowTrap* NewShark = GetWorld()->SpawnActor<AArrowTrap>(SharkClass, SpawnLocation, SpawnRotation, SpawnParams);

}