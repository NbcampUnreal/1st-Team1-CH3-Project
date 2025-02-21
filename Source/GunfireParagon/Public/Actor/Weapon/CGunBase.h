#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/Bullet/BulletBase.h"
#include "CGunBase.generated.h"

class ABulletPool;


UCLASS()
class GUNFIREPARAGON_API ACGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGunBase();

protected:
	virtual void BeginPlay() override;

	virtual void Fire();

	
	// 총기 딜레이에 따라 발사 가능 여부 확인
	bool CanFire() const;

	//나중에 바인딩할때 마우스 떼면 그만 쏘도록
	void StopFire();
	void SetIsFire(); 
	//  탄창이 비었는지 확인
	bool IsAmmoEmpty() const;

	//  탄창을 재장전
	virtual void Reload();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunStatus")
	int32 MaxAmmo = 30; //  최대 탄창 용량

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GunStatus")
	int32 CurrentAmmo; //  현재 탄창

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunStatus")
	ABulletPool* BulletPool;

	//  자동사격 모드 (true면 자동사격, false면 단발)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunStatus")
	bool bIsAutoFire = false;

	//딜레이동안 사격금지하기위한 bool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunStatus")
	bool bCanFire  = true;
	
	UPROPERTY()
	FTimerHandle FireTimer;

	UPROPERTY()
	FTimerHandle AutoFireTimer;
	//  발사 타이머 (자동 발사용)
	UPROPERTY()
	FTimerHandle TestFireTimer;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	EAmmoType AmmoType; 
	
};
