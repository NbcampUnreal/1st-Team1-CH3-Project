#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dom/JsonObject.h"  // JSON 파싱용
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/DataTable.h"
#include "FPSDataTables.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GunDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Grade;
};

UCLASS()
class GUNFIREPARAGON_API AFPSDataTables : public AActor
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

private:
	// CSV에서 DataTable을 로드하는 함수
	void LoadWeaponDataFromCSV();
	void LoadWeaponDataFromJSON();


public:

	//  데이터 테이블에서 특정 무기 데이터를 가져오는 함수 추가
	FWeaponData* GetWeaponDataByKey(int32 Key);

	//  블루프린트에서 설정할 수 있도록 데이터 테이블 변수 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	TMap<int32, FWeaponData*> WeaponDataMap;
	
};