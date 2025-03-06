// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameMode/TrapPortalAction.h"
#include "GameMode/TrapPortalTypes.h"
#include "TrapPortal.generated.h"

class UBoxComponent;

UCLASS()
class GUNFIREPARAGON_API ATrapPortal : public AActor
{
	GENERATED_BODY()

public:

	ATrapPortal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	UStaticMeshComponent* PortalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UParticleSystemComponent* PortalEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	ETrapPortalTypes PortalType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	USoundBase* PortalSound;

	UFUNCTION()
	void OnPortalOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void TravelToTravel();
	void ReturnToStage();
};