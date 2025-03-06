#include "Actor/Trap/DropManager.h"
#include "Kismet/GameplayStatics.h"
#include "FPSDataTables.h"
#include "Actor/Weapon/CGunBase.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"

ADropManager::ADropManager()
{
    
    PrimaryActorTick.bCanEverTick = false;
   
}

void ADropManager::BeginPlay()
{
    Super::BeginPlay();
    DataTables = Cast<AFPSDataTables>(UGameplayStatics::GetActorOfClass(GetWorld(), AFPSDataTables::StaticClass()));

    if (!DataTables)
    {
        DataTables = GetWorld()->SpawnActor<AFPSDataTables>();
    }
}

// 랜덤한 무기 데이터 가져오기
FWeaponData* ADropManager::GetRandomWeaponData()
{
    if (!DataTables)
    {   
        UE_LOG(LogTemp, Error, TEXT("DropManager: DataTables is NULL!"));
        return nullptr;
    }

    if (!DataTables->WeaponDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("DropManager: WeaponDataTable is NULL!"));
        return nullptr;
    }

    TArray<FName> RowNames = DataTables->WeaponDataTable->GetRowNames();
    if (RowNames.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("DropManager: WeaponDataTable has no rows!"));
        return nullptr;
    }

    // 랜덤한 행 선택
    int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
    return DataTables->WeaponDataTable->FindRow<FWeaponData>(RowNames[RandomIndex], TEXT(""));
}

// 랜덤 아이템 드롭
void ADropManager::RandomItemDrop(FVector SpawnLocation)
{
    // 랜덤한 무기 데이터 가져오기
    FWeaponData* WeaponData = GetRandomWeaponData();
    if (!WeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("DropManager: WeaponData is NULL!"));
        return;
    }

    // 블루프린트 클래스 확인
    TSubclassOf<ACGunBase> SelectedGunBlueprint = nullptr;
    for (const FWeaponSpawnInfo& WeaponInfo : WeaponList)
    {
        if (WeaponInfo.WeaponKey == WeaponData->Key)
        {
            SelectedGunBlueprint = WeaponInfo.GunBlueprint;
            break;
        }
    }
    
    if (!SelectedGunBlueprint)
    {
        return;
    }
    
    ACGunBase* SpawnedGun = GetWorld()->SpawnActor<ACGunBase>(SelectedGunBlueprint, SpawnLocation, FRotator::ZeroRotator);
    if (!SpawnedGun)
    {
        return;
    }

    SpawnedGun->WeaponDataKey = WeaponData->Key;
    SpawnedGun->SetWeaponData(WeaponData->Key);
    SpawnedGun->SetIsDrop(true);
    
    UStaticMeshComponent* GunMeshComp = Cast<UStaticMeshComponent>(SpawnedGun->GetComponentByClass(UStaticMeshComponent::StaticClass()));
    if (GunMeshComp)
    {
        GunMeshComp->SetSimulatePhysics(true);
        GunMeshComp->SetEnableGravity(true);

        FVector RandomForce = FVector(
            FMath::RandRange(-500.0f, 500.0f),
            FMath::RandRange(-500.0f, 500.0f),
            FMath::RandRange(300.0f, 800.0f) 
        );
        GunMeshComp->AddImpulse(RandomForce, NAME_None, true);
    }
}