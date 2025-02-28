// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MinimapTracker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorLocation, ACharacter* ,Target, float, Distance);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIREPARAGON_API UMinimapTracker : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMinimapTracker();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FVector LastLocation;
	void CheckLocationChange(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);

	UFUNCTION()
	void OnOwnerMoved(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);
		
public:
	static FOnActorLocation OnActorLocation;
};
