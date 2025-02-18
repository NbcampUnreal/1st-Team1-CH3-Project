#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGunBase.generated.h"

UCLASS()
class GUNFIREPARAGON_API ACGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGunBase();

protected:
	virtual void BeginPlay() override;

	virtual void Fire(FVector StartPos, FVector Direction,float Damage);

	FVector SpreadDirection(const FVector OriginDirection) const;


//총기의 스텟관련 변수들,assetraw에서 데이터 받아와서저장시킬예정
protected:


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunMesh")
	UStaticMeshComponent* GunMesh;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
	FVector MuzzleSpot;
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
    float Damage = 10.0f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
    float GunDelay = 10.0f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
	float GunSpread = 5.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
	float GunSpeed = 100.0f;


};
