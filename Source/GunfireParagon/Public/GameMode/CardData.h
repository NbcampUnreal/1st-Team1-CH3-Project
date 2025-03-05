// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "CardData.generated.h"

UENUM(BlueprintType)
enum class ECardEffectType : uint8
{
    AttackPowerIncrease UMETA(DisplayName = "Attack Power Increase"),
    AttackSpeedIncrease UMETA(DisplayName = "Attack Speed Increase"),
    MoveSpeedIncrease UMETA(DisplayName = "Move Speed Increase"),
    ShieldAmountIncrease UMETA(DisplayName = "Shield Amount Increase"),
    ShieldRegeneTimeDecrease UMETA(DisplayName = "Shield Regene Time Decrease"),
    ShieldRateIncrease UMETA(DisplayName = "Shield Rate Increase"),
    MaxHealthIncrease UMETA(DisplayName = "Max Health Increase"),
    DashCoolTimeDecrease UMETA(DisplayName = "Dash CoolTime Decrese"),
    LastIndex UMETA(Hidden) // Rand 시 최대값 탐색용
};

UENUM(BlueprintType)
enum class EEffectValueType : uint8
{
    FlatValue UMETA(DisplayName = "Flat Value"),
    Percentage UMETA(DisplayName = "Percentage"),
    Duration UMETA(DisplayName = "Duration")
};

UENUM(BlueprintType)
enum class ECardRarity : uint8
{
    Common UMETA(DisplayName = "Common"),
    Rare UMETA(DisplayName = "Rare"),
    Legendary UMETA(DisplayName = "Legendary"),
    LastIndex UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCardEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECardEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEffectValueType ValueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectValues;

    FCardEffect()
        : EffectType(ECardEffectType::AttackPowerIncrease)
        , ValueType(EEffectValueType::FlatValue)
        , EffectValues(0.0f) {
    }

    FCardEffect(ECardEffectType InEffectType, EEffectValueType InValueType, float InEffectValues)
        : EffectType(InEffectType)
        , ValueType(InValueType)
        , EffectValues(InEffectValues) {
    }
};

USTRUCT(BlueprintType)
struct FCardDataTable : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CardName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CardDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECardRarity Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCardEffect CardEffect;
};

USTRUCT(BlueprintType)
struct FCardPKDataTable : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PrimaryCardKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECardEffectType CardType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCardEffect CardEffect;
};

UCLASS(Blueprintable)
class GUNFIREPARAGON_API UCardData : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CardName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CardDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECardRarity Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCardEffect CardEffect;
};
