// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Trap/MovingSpotLight.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMovingSpotLight::AMovingSpotLight()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	MovementArea = CreateDefaultSubobject<UBoxComponent>(TEXT("MovementArea"));
	MovementArea->SetupAttachment(RootComponent);
	MovementArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MovementArea->SetBoxExtent(FVector(1000.0f, 1000.0f, 500.0f)); 

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(RootComponent);
	SpotLight->SetIntensity(1000.0f);
	SpotLight->SetLightColor(FLinearColor(1.0f, 0.8f, 0.6f)); 
	SpotLight->SetAttenuationRadius(1000.0f);
	SpotLight->SetOuterConeAngle(30.0f);
	SpotLight->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f)); 
}

void AMovingSpotLight::BeginPlay()
{
	Super::BeginPlay();

	SetNewTargetLocation();

	GetWorldTimerManager().SetTimer(ChangeTargetTimer, this, &AMovingSpotLight::SetNewTargetLocation, ChangeInterval, true);
}

void AMovingSpotLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed * 0.01f);
	SetActorLocation(NewLocation);
}

void AMovingSpotLight::SetNewTargetLocation()
{
	FVector BoxExtent = MovementArea->GetScaledBoxExtent();
	FVector Origin = GetActorLocation();

	float RandomX = UKismetMathLibrary::RandomFloatInRange(-BoxExtent.X, BoxExtent.X);
	float RandomY = UKismetMathLibrary::RandomFloatInRange(-BoxExtent.Y, BoxExtent.Y);

	TargetLocation = Origin + FVector(RandomX, RandomY,0.0f);
}