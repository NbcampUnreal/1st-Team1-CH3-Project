// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Utility/MinimapManager.h"

void UMinimapManager::Initialize()
{
	TargetLocations.Empty();
}

void UMinimapManager::RegisterTarget(AActor* Target)
{
	if(!GetWorld())
	{
		UE_LOG(LogTemp, Display, TEXT("World Is Null"));
		return;
	}
	if (IsValid(Target) && !TargetLocations.Contains(Target))
	{
		TargetLocations.Add(Target, Target->GetActorLocation());

		if (!UpdateTimer.IsValid())
		{
			UpdateTargetsLocation();
		}
		UE_LOG(LogTemp, Display, TEXT("Register Test : %s"), *Target->GetName());
	}
}

void UMinimapManager::UnRegisterTarget(AActor* Target)
{
	if (TargetLocations.Contains(Target))
	{
		TargetLocations.Remove(Target);
		OnTargetRemoved.Broadcast(Target);
	}

	if (TargetLocations.Num() == 0)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(UpdateTimer);
		}
	}
}

void UMinimapManager::RecvLocationToBroadCast()
{
	TArray<FMinimapTargetLocation> TargetData;

	for (const auto& Target : TargetLocations)
	{
		if (Target.Key)
		{
			FMinimapTargetLocation Data;
			Data.TargetActor = Target.Key;
			Data.Location = Target.Key->GetActorLocation();
			TargetData.Add(Data);
		}
		else
		{
			UnRegisterTarget(Target.Key);
		}
	}

	OnAllLocationsUpdated.Broadcast(TargetData);
}

void UMinimapManager::UpdateTargetsLocation()
{
	float UpdateCycleTime = 0.1f;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UMinimapManager::RecvLocationToBroadCast, UpdateCycleTime, true);
	}
}