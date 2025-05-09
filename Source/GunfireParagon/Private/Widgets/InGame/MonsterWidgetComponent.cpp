// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/MonsterWidgetComponent.h"
#include "Widgets\InGame\IngameTargetHUD.h"
#include "Kismet\GameplayStatics.h"
#include "GameFramework\Actor.h"
#include "GameFramework\Character.h"
#include "AI\Enemy/BaseEnemy.h"

UMonsterWidgetComponent::UMonsterWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetWidgetSpace(EWidgetSpace::World);
	SetDrawSize(FVector2D(200.f, 50.f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	bDrawAtDesiredSize = false;
	IsSHowHUD = false;
}

void UMonsterWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TargetHUDClass)
	{
		SetWidgetClass(TargetHUDClass);
		InitWidget();
	}

	if (GetWidget() && GetWidget()->IsA(TargetHUDClass))
	{
		TargetHUD = Cast<UIngameTargetHUD>(GetWidget());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error : Target HUD Not Binding."));
	}

	if (ABaseEnemy* Target = Cast<ABaseEnemy>(GetOwner()))
	{
		ConvertTickRate();
		SetVisibility(IsSHowHUD, true);
		if (!(Target->OnTargetHPChanged.IsBound()))
		{
			Target->OnTargetHPChanged.AddDynamic(this, &UMonsterWidgetComponent::UpdateHP);
		}
		
		if (!Target->OnTargetName.IsBound())
		{
			Target->OnTargetName.AddDynamic(this, &UMonsterWidgetComponent::UpdateName);
		}
	}
}

void UMonsterWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateScale(GetPlayerToDistance());
	UpdateForwardVector();
	ConvertTickRate();
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

	IsSHowHUD = (CurrentHP == MaxHP) ? false : true;
	SetVisibility(IsSHowHUD, true);


	float VisibleDuration = 3.f;
	GetWorld()->GetTimerManager().SetTimer(VisibleTimer, this, &UMonsterWidgetComponent::ConvertVisibilityMode, VisibleDuration, false);
}

void UMonsterWidgetComponent::UpdateName(FString TargetName)
{
	if (TargetHUD)
	{
		TargetHUD->UpdateTargetName(TargetName);
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
	SetVisibility(false, true);

	if (VisibleTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(VisibleTimer);
	}
}

void UMonsterWidgetComponent::ConvertTickRate()
{
	ABaseEnemy* Target = Cast<ABaseEnemy>(GetOwner());
	if (!Target) return;

	bool IsTargetDead = Target->bIsDead;
	SetComponentTickEnabled(!IsTargetDead);
	if (IsTargetDead)
	{
		IsSHowHUD = !Target->bIsDead;
		SetVisibility(IsSHowHUD, true);
	}
}
