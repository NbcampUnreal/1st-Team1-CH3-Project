// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TrapPortal.h"
#include "GameMode/FPSGameMode.h"
#include "GameMode/FPSGameInstance.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ATrapPortal::ATrapPortal()
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
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATrapPortal::OnPortalOverlap);

	PortalEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(SceneComponent);

	PortalType = ETrapPortalTypes::TravelToTrap;
}

void ATrapPortal::OnPortalOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	APlayerCharacter* FPSCharacter = Cast<APlayerCharacter>(OtherActor);

	if (FPSCharacter && FPSCharacter->IsA(APlayerCharacter::StaticClass()))
	{
		AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>((GetWorld()->GetAuthGameMode()));
		UFPSGameInstance* FPSGameInstance = Cast<UFPSGameInstance>(UGameplayStatics::GetGameInstance(this));
		if (FPSGameMode && FPSGameInstance)
		{
			if (PortalType == ETrapPortalTypes::TravelToTrap)
			{
				if (PortalSound)
				{
					UGameplayStatics::PlaySound2D(this, PortalSound);
				}

				FTimerHandle StageLoadTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					StageLoadTimerHandle,
					this,
					&ATrapPortal::TravelToTravel,
					2.0f,
					false
				);
			}
			else if (PortalType == ETrapPortalTypes::ReturnToStage)
			{
				if (PortalSound)
				{
					UGameplayStatics::PlaySound2D(this, PortalSound);
				}

				FTimerHandle StageLoadTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					StageLoadTimerHandle,
					this,
					&ATrapPortal::ReturnToStage,
					2.0f,
					false
				);
			}
		}
	}
}

void ATrapPortal::TravelToTravel()
{
	AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>((GetWorld()->GetAuthGameMode()));
	if (FPSGameMode)
	{

		FPSGameMode->TravelToLevel("Stage_Trap_01");
	}
}

void ATrapPortal::ReturnToStage()
{
	AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>((GetWorld()->GetAuthGameMode()));
	if (FPSGameMode)
	{
		FPSGameMode->ReturnToPreviousLevel();
	}
}




