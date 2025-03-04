#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "Components//SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "ArrowTrap.generated.h"

class UPointLightComponent;

UCLASS()
class GUNFIREPARAGON_API AArrowTrap : public ATrapBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USceneComponent* RootScene;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Components")
	USkeletalMeshComponent* ArrowMesh; 

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Components")
	USpotLightComponent* SpotLight; 

	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveSpeed = 300.0f; 

	UPROPERTY(EditAnywhere, Category="Movement")
	float Amplitude = 50.0f; 

	UPROPERTY(EditAnywhere, Category="Movement")
	float Frequency = 2.0f;

	
	UPROPERTY(EditAnywhere, Category="Light")
	float LightIntensity = 1500.0f;
	UPROPERTY(EditAnywhere, Category="Light")
	float LightHeight = 400.0f; 
	UPROPERTY(EditAnywhere, Category="Light")
	float LightAngle = 45.0f;
	float LightDistance = 1000.0f;
	
	FVector InitialPosition; 
	float RunningTime;  

public:
	AArrowTrap();

protected:
	virtual void BeginPlay() override;
	virtual void ActivateTrap() override;
	virtual void Tick(float DeltaTime) override;

private:
	void MoveTowardsPlayer(float DeltaTime); 
	AActor* FindPlayer();  

	virtual void OnTrapTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						 bool bFromSweep, const FHitResult& SweepResult) override;
};