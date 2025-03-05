#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "FPSDataTables.h"
#include "DropManager.generated.h"

// 총을 스폰할 구조체
USTRUCT(BlueprintType)
struct FWeaponSpawnInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="SpawnInfo")
	TSubclassOf<class ACGunBase> GunBlueprint; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="SpawnInfo")
	int32 WeaponKey;
};

UCLASS()
class GUNFIREPARAGON_API ADropManager : public AActor
{
	GENERATED_BODY()
    
public:    
	ADropManager();

protected:
	virtual void BeginPlay() override;

public:    

	// 랜덤 아이템 드롭 함수
	UFUNCTION(BlueprintCallable, Category = "Drop")
	void RandomItemDrop(FVector SpawnLocation);

	// 데이터 테이블 참조
	UPROPERTY(EditAnywhere, Category = "Drop")
	class AFPSDataTables* DataTables;
	
private:
	// 드롭할 무기 목록
	UPROPERTY(EditAnywhere, Category = "Drop")
	TArray<FWeaponSpawnInfo> WeaponList;


	// 랜덤 무기 선택 함수
	FWeaponData* GetRandomWeaponData();
};