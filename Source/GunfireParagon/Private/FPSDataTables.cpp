#include "FPSDataTables.h"
#include "Engine/DataTable.h"


void AFPSDataTables::BeginPlay()
{
	Super::BeginPlay();
	LoadWeaponDataFromCSV();
}

void AFPSDataTables::LoadWeaponDataFromCSV()
{
	FString FilePath = FPaths::ProjectContentDir() + TEXT("DataAsset/ItemData.csv");

	// 파일 읽기
	FString CSVData;
	if (!FFileHelper::LoadFileToString(CSVData, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("CSV 파일을 읽을 수 없습니다: %s"), *FilePath);
		return;
	}

	// DataTable 객체가 없으면 새로 생성
	if (!WeaponDataTable)
	{
		WeaponDataTable = NewObject<UDataTable>(this, UDataTable::StaticClass());
		WeaponDataTable->RowStruct = FWeaponDataTable::StaticStruct();
	}
}


// 데이터 테이블에서 특정 키를 가진 무기 정보를 가져오는 함수
FWeaponDataTable* AFPSDataTables::GetWeaponDataByKey(int32 Key)
{
	if (!WeaponDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDataTable이 설정되지 않았습니다!"));
		return nullptr;
	}

	// Key를 FName으로 변환하여 검색
	FName RowName = FName(*FString::FromInt(Key));
	FWeaponDataTable* FoundWeaponData = WeaponDataTable->FindRow<FWeaponDataTable>(RowName, TEXT(""));

	if (!FoundWeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Key %d에 해당하는 무기 데이터를 찾을 수 없습니다."), Key);
		return nullptr;
	}

	return FoundWeaponData;  // 포인터 반환
}