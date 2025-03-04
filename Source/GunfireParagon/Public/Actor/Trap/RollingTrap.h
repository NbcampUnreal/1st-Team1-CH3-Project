#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "Components//SphereComponent.h"
#include "Components//SkeletalMeshComponent.h"
#include "RollingTrap.generated.h"

UCLASS()
class GUNFIREPARAGON_API ARollingTrap : public ATrapBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Trap")
	USphereComponent* DamageCollider; 

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Trap")
	USkeletalMeshComponent* BoulderMesh;  

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Trap")
	float MoveSpeed = 600.0f; 

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Trap")
	float RotationSpeed = 200.0f;  

	bool bHasHitGround = false;

	FVector StartLocation;

public:
	ARollingTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void ActivateTrap() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnOverlapWithDestroyBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult);
};