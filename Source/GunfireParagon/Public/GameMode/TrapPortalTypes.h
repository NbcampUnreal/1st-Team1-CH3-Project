// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapPortalTypes.generated.h"


UENUM(BlueprintType)
enum class ETrapPortalTypes : uint8
{
	TravelToTrap UMETA(DisplayName = "TravelToTrap"),
	ReturnToStage UMETA(DisplayName = "ReturnToStage")
};
