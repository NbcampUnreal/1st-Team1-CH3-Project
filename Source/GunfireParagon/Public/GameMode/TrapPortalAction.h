// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapPortalAction.generated.h"


UENUM(BlueprintType)
enum class ETrapPortalAction : uint8
{
	Entry UMETA(DisplayName = "Entry"),
	Exit UMETA(DisplayName = "Exit")
};