// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"


ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector Extend = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	return Origin + FVector(
		FMath::FRandRange(-Extend.X, Extend.X),
		FMath::FRandRange(-Extend.Y, Extend.Y),
		FMath::FRandRange(-Extend.Z, Extend.Z)
	);
}

// 안전한 위치 스폰, 충돌 감지
FVector ASpawnVolume::GetSafeSpawnPoint() const
{
    FVector SpawnPoint;
    int32 MaxAttempts = 10;
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    for (int32 i = 0; i < MaxAttempts; i++)
    {
        SpawnPoint = GetRandomPointInVolume();

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        FCollisionObjectQueryParams ObjectQueryParams;

        QueryParams.AddIgnoredActor(this);
        ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); 
        ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

        bool bBlocked = GetWorld()->SweepSingleByObjectType(
            HitResult,
            SpawnPoint,
            SpawnPoint + FVector(0, 0, 10),
            FQuat::Identity,
            ObjectQueryParams,  
            FCollisionShape::MakeSphere(50.0f), // 추후 Ai 크기 가져와야 할듯
            QueryParams
        );

        // Navmesh 위에 스폰되도록 설정
        if (!bBlocked && NavSystem)
        {
            FNavLocation NavLocation;
            if (NavSystem->ProjectPointToNavigation(SpawnPoint, NavLocation))
            {
                UE_LOG(LogTemp, Warning, TEXT("Spawned AI at : %s"), *NavLocation.Location.ToString());
                return NavLocation.Location;
            }
        }
    }

    return SpawnPoint;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
    if (!ItemClass) return nullptr;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ItemClass,
        GetSafeSpawnPoint(),
        FRotator::ZeroRotator
    );

    return SpawnedActor;
}
