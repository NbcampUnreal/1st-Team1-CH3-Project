#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingSpotLight.generated.h"

class USpotLightComponent;
class UBoxComponent;

UCLASS()
class GUNFIREPARAGON_API AMovingSpotLight : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingSpotLight();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* MovementArea; 

	UPROPERTY(VisibleAnywhere, Category="Components")
	USpotLightComponent* SpotLight; 

	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MinZ = 100.0f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxZ = 500.0f;  

	UPROPERTY(EditAnywhere, Category="Movement")
	float ChangeInterval = 5.0f; 

	FVector TargetLocation; 
	FTimerHandle ChangeTargetTimer;  

	void SetNewTargetLocation();
};