// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameMode/TrapPortalTypes.h"
#include "GameMode/TrapPortalAction.h"
#include "TrapPortalPoint.generated.h"


UCLASS()
class GUNFIREPARAGON_API ATrapPortalPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	ATrapPortalPoint();

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	// ETrapPortalTypes PortalType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	ETrapPortalAction PortalAction;
};
