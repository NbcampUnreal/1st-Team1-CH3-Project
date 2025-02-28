// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/MonsterWidgetComponent.h"
#include "Widgets\InGame\IngameTargetHUD.h"
#include "Kismet\GameplayStatics.h"
#include "GameFramework\Actor.h"
#include "GameFramework\Character.h"

UMonsterWidgetComponent::UMonsterWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetWidgetSpace(EWidgetSpace::World);
	SetDrawSize(FVector2D(200.f, 50.f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bDrawAtDesiredSize = false;
}

void UMonsterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWidget() && GetWidget()->IsA(TargetHUDClass))
	{
		TargetHUD = Cast<UIngameTargetHUD>(GetWidget());
	}
}

void UMonsterWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float PlayerToDistance = GetPlayerToDistance();
	UpdateScale(PlayerToDistance);
	UpdateForwardVector();
}

void UMonsterWidgetComponent::UpdateScale(float Distance)
{
	float MaxDistance = 1000.f;
	float ScaleFactor = FMath::Clamp(MaxDistance / Distance, 0.5f, 1.0f);
	SetRelativeScale3D(FVector(ScaleFactor, ScaleFactor, ScaleFactor));
}

void UMonsterWidgetComponent::UpdateHP(float CurrentHP, float MaxHP)
{
	if (TargetHUD)
	{
		TargetHUD->UpdateHP(CurrentHP, MaxHP);
	}
}

void UMonsterWidgetComponent::UpdateForwardVector()
{
	if (!GetSinglePlayer()) return;

	FVector DirectionToPlayer = GetSinglePlayer()->GetActorLocation() - GetComponentLocation();
	DirectionToPlayer.Normalize();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
	SetWorldRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
}

ACharacter* UMonsterWidgetComponent::GetSinglePlayer()
{
	return Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

float UMonsterWidgetComponent::GetPlayerToDistance()
{
	ACharacter* Player = GetSinglePlayer();
	ACharacter* Target = Cast<ACharacter>(GetOwner());

	if (!Player || !Target) return -1.f;

	return FVector::Dist(Target->GetActorLocation(), Player->GetActorLocation());
}

void UMonsterWidgetComponent::ConvertVisibilityMode()
{
	ACharacter* Target = Cast<ACharacter>(GetOwner());
	if (!Target) return;

	bool Visibility = !(Target->IsHidden());
	SetComponentTickEnabled(Visibility);
	SetVisibility(Visibility, true);
}
