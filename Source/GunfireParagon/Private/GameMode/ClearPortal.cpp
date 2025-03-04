// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ClearPortal.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameMode/FPSGameInstance.h"


AClearPortal::AClearPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(SceneComponent);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(SceneComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AClearPortal::OnPortalOverlap);

	PortalEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(SceneComponent);
}

void AClearPortal::OnPortalOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bHasActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadNextStage() already triggered!"));
		return;
	}

	APlayerCharacter* FPSCharacter = Cast<APlayerCharacter>(OtherActor);
	if (FPSCharacter && FPSCharacter->IsA(APlayerCharacter::StaticClass()))
	{
		bHasActivated = true;
		LoadNextStage();
	}
}

void AClearPortal::LoadNextStage()
{
	UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(GetGameInstance());

	if (FPSGameInstance)
	{
		FPSGameInstance->LoadNextStage();
	}
}


