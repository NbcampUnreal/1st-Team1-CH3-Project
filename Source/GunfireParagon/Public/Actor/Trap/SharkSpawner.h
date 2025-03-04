// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/Trap/ArrowTrap.h"
#include "Components/BoxComponent.h"
#include "SharkSpawner.generated.h"

UCLASS()
class GUNFIREPARAGON_API ASharkSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* TriggerBox_Entry; 

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* TriggerBox_Exit;

	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AArrowTrap> SharkClass; 

	UPROPERTY(EditAnywhere, Category="Spawning")
	float SpawnInterval = 5.0f;

	UPROPERTY(EditAnywhere, Category="Spawning")
	float SharkLifetime = 20.0f;  

	FTimerHandle SpawnTimerHandle;
	bool bIsActive = false; 

public:
	ASharkSpawner();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnEntryTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExitTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						 bool bFromSweep, const FHitResult& SweepResult);

	void SpawnShark();

};
