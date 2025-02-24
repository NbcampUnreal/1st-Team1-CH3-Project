#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "FPSDataTables.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	FWeaponDataTable()
	{
		RowName = TEXT("Weapon1");
		Key = 1;
		Name = TEXT("Weapon");
		AttackPower = 10.0f;
		AttackSpeed = 10.0f;
		MaxAmmo = 30;
		GunDelay = 0.1f;
		Grade = TEXT("Normal");
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName RowName;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float GunDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
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


public:

	//  데이터 테이블에서 특정 무기 데이터를 가져오는 함수 추가
	FWeaponDataTable* GetWeaponDataByKey(int32 Key);

	//  블루프린트에서 설정할 수 있도록 데이터 테이블 변수 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;
	
};