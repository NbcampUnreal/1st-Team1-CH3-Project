#include "FPSDataTables.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


void AFPSDataTables::BeginPlay()
{
	Super::BeginPlay();
	LoadWeaponDataFromCSV();
	//LoadWeaponDataFromJSON();
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

void AFPSDataTables::LoadWeaponDataFromJSON()
{
	// FString FilePath = FPaths::ProjectContentDir() + TEXT("DataAsset/WeaponData.json");
	//
	// FString JsonString;
	// if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("JSON 파일을 읽을 수 없습니다: %s"), *FilePath);
	// 	return;
	// }
	//
	// TSharedPtr<FJsonObject> JsonObject;
	// TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	//
	// if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("JSON 파싱 실패!"));
	// 	return;
	// }
	//
	// TArray<TSharedPtr<FJsonValue>> WeaponArray = JsonObject->GetArrayField("Weapons");
	//
	// for (TSharedPtr<FJsonValue> Value : WeaponArray)
	// {
	// 	TSharedPtr<FJsonObject> WeaponObject = Value->AsObject();
	// 	if (!WeaponObject) continue;
	//
	// 	FWeaponData WeaponData;
	// 	WeaponData.Name = WeaponObject->GetStringField("Name");
	// 	WeaponData.Key = WeaponObject->GetIntegerField("Key");
	// 	WeaponData.AttackPower = WeaponObject->GetNumberField("AttackPower");
	// 	WeaponData.AttackSpeed = WeaponObject->GetNumberField("AttackSpeed");
	// 	WeaponData.MaxAmmo = WeaponObject->GetIntegerField("MaxAmmo");
	// 	WeaponData.GunDelay = WeaponObject->GetNumberField("GunDelay");
	// 	WeaponData.Grade = WeaponObject->GetStringField("Grade");
	//
	// 	WeaponDataMap.Add(WeaponData.Key, &WeaponData);
	// 	UE_LOG(LogTemp, Warning, TEXT("무기 데이터 추가됨: Key = %d, Name = %s"), WeaponData.Key, *WeaponData.Name);
	// }
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