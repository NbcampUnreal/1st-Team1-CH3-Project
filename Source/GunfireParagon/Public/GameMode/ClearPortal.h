// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClearPortal.generated.h"

class UBoxComponent;

UCLASS()
class GUNFIREPARAGON_API AClearPortal : public AActor
{
	GENERATED_BODY()
	
public:	

	AClearPortal();
	void LoadNextStage();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	UStaticMeshComponent* PortalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	UParticleSystemComponent* PortalEffect;
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

	bool bHasActivated = false;
};
