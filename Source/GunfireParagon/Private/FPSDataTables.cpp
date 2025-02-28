#include "FPSDataTables.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


AFPSDataTables::AFPSDataTables()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Game/DataAsset/ItemTable.ItemTable"));
	if (WeaponDataTableFinder.Succeeded())
	{
		WeaponDataTable = WeaponDataTableFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("✅ 데이터 테이블 로드 성공: %s"), *WeaponDataTable->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 데이터 테이블 로드 실패! 경로를 확인하세요."));
		return;
	}
}

void AFPSDataTables::BeginPlay()
{
	Super::BeginPlay();
	
	
	LoadWeaponDataFromCSV();
}

void AFPSDataTables::LoadWeaponDataFromCSV()
{
	
	// DataTable 객체가 없으면 새로 생성
	if (!WeaponDataTable)
	{
		WeaponDataTable = NewObject<UDataTable>(this, UDataTable::StaticClass());
		WeaponDataTable->RowStruct = FWeaponData::StaticStruct();
	}
	
	// 데이터 테이블에서 모든 행을 가져와서 Key 기반으로 맵에 저장
	TArray<FName> RowNames = WeaponDataTable->GetRowNames();
	for (FName RowName : RowNames)
	{
		FWeaponData* WeaponData = WeaponDataTable->FindRow<FWeaponData>(RowName, TEXT(""));
		if (WeaponData)
		{
			WeaponDataMap.Add(WeaponData->Key, WeaponData);
		}
	}
}



// 데이터 테이블에서 특정 키를 가진 무기 정보를 가져오는 함수
FWeaponData* AFPSDataTables::GetWeaponDataByKey(int32 Key)
{
	if (!WeaponDataMap.Contains(Key))
	{
		UE_LOG(LogTemp, Warning, TEXT("Key %d에 해당하는 무기 데이터를 찾을 수 없습니다."), Key);
		return nullptr;
	}

	return WeaponDataMap[Key];
}