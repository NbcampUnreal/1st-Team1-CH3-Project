#pragma once

#include "CoreMinimal.h"
#include "TrapBase.h"
#include "SpikeTrap.generated.h"

UCLASS()
class GUNFIREPARAGON_API ASpikeTrap : public ATrapBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category="Trap")
	TArray<UStaticMeshComponent*> Spikes;  // 9개의 가시

	UPROPERTY(EditAnywhere, Category="Trap")
	float InitialDelay = 1.0f; 

	UPROPERTY(EditAnywhere, Category="Trap")
	float ActivationDelay = 2.0f;  // 가시가 올라오는 시간

	UPROPERTY(EditAnywhere, Category="Trap")
	float RetractDelay = 3.0f;  // 가시가 내려가는 시간

	UPROPERTY(EditAnywhere, Category="Trap")
	float SpikeHeight = 50.0f;
	UPROPERTY(EditAnywhere, Category="Trap")
	FVector Startheight;;
	

	UPROPERTY(EditAnywhere, Category="Trap")
	float ShakeIntensity = 5.0f; 

	UPROPERTY(EditAnywhere, Category="Trap")
	float ShakeDuration = 1.5f; 

	UPROPERTY(EditAnywhere, Category="Trap")
	float GridSize = 50.0f;
	
	bool bIsSpikesUp = false;  
	bool bHasDealtDamage = false;  

	FTimerHandle SpikeTimerHandle;
	FTimerHandle ShakeTimerHandle;
	int ShakeCount = 0; 

public:
	ASpikeTrap();

protected:
	virtual void BeginPlay() override;

	void StartTrapSequence(); 

	void ShakeSpikes();  
	void RaiseSpikes();  
	void LowerSpikes();  

	UFUNCTION()
	void OnSpikeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};