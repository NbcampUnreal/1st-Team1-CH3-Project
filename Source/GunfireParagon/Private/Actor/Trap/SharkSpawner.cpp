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
		UE_LOG(LogTemp, Warning, TEXT("방 입구 트리거 활성화! 상어 스폰 시작"));
		bIsActive = true;
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASharkSpawner::SpawnShark, SpawnInterval, true);
	}
}
void ASharkSpawner::OnExitTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player") && bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("방 출구 트리거 활성화! 상어가 플레이어를 찾지 못함"));

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

	if (ExistingSharks.Num() >= 5)
	{
		UE_LOG(LogTemp, Warning, TEXT("상어 스폰 제한 도달! 현재 상어 개수: %d"), ExistingSharks.Num());
		return;  
	}

	UE_LOG(LogTemp, Warning, TEXT("상어 스폰 시작 (현재 상어 수: %d)"), ExistingSharks.Num());

	FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AArrowTrap* NewShark = GetWorld()->SpawnActor<AArrowTrap>(SharkClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (!NewShark)
	{
		UE_LOG(LogTemp, Error, TEXT("상어 스폰 실패: SpawnActor 반환값이 nullptr"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("상어 스폰 성공: %s (현재 상어 수: %d)"), *NewShark->GetName(), ExistingSharks.Num() + 1);
	}
}