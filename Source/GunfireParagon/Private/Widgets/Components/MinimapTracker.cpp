// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/MinimapTracker.h"
#include "Kismet\GameplayStatics.h"
#include "GameFramework\Character.h"
#include "GameFramework\PawnMovementComponent.h"

FOnActorLocation UMinimapTracker::OnActorLocation;

UMinimapTracker::UMinimapTracker()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMinimapTracker::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		LastLocation = Character->GetActorLocation();

		Character->OnCharacterMovementUpdated.AddDynamic(this, &UMinimapTracker::OnOwnerMoved);
	}
}

void UMinimapTracker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		Character->OnCharacterMovementUpdated.RemoveDynamic(this, &UMinimapTracker::OnOwnerMoved);
	}
}

void UMinimapTracker::CheckLocationChange(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!OwnerCharacter || !PlayerCharacter) return;

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	float Distance = FVector::Dist(CurrentLocation, PlayerCharacter->GetActorLocation());

	if (!CurrentLocation.Equals(LastLocation, 5.0f))
	{
		OnActorLocation.Broadcast(OwnerCharacter, Distance);
		LastLocation = CurrentLocation;
	}
}

void UMinimapTracker::OnOwnerMoved(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	CheckLocationChange(DeltaSeconds, OldLocation, OldVelocity);
}
