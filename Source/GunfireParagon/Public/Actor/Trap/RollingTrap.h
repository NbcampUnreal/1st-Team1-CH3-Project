#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "RollingTrap.generated.h"

class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class GUNFIREPARAGON_API ARollingTrap : public ATrapBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category="Trap")
	USphereComponent* DamageCollider;

	UPROPERTY(VisibleAnywhere, Category="Trap")
	USkeletalMeshComponent* BoulderMesh;

	UPROPERTY(EditAnywhere, Category="Trap")
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category="Trap")
	float RotationSpeed = 2000.0f;  

	UPROPERTY(EditAnywhere, Category="Trap")
	float GravityScale = 980.0f;  
	bool bHasHitGround = false;  
	FVector Velocity;  
	FVector StartLocation;
	FTimerHandle DropTimerHandle; 
	UPROPERTY(EditAnywhere, Category="Trap")
	float DropInterval = 2.0f;  
	bool bCanFall = false;

public:
	ARollingTrap();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void ActivateTrap() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapWithDestroyBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult);

	void StartFalling();
};