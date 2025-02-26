#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/Bullet/BulletBase.h"
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "CGunBase.generated.h"

class ABulletPool;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Sniper UMETA(DisplayName = "Sniper"),
	Rocket UMETA(DisplayName = "Rocket"),
	Default UMETA(DisplayName = "Default")  // 기본값
};

UCLASS()
class GUNFIREPARAGON_API ACGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGunBase();

	virtual void Fire();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunMesh")
	UStaticMeshComponent* GunMesh;
	bool bWasDropped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootComp;


protected:
	virtual void BeginPlay() override;

	//총기 딜레이에 따라 발사 가능 여부 확인
	bool CanFire();

	//나중에 바인딩할때 마우스 떼면 그만 쏘도록
	void StopFire();
	void SetIsFire(); 
	//탄창이 비었는지 확인
	bool IsAmmoEmpty() ;

	virtual void Reload();
	
	FVector SpreadDirection(const FVector OriginDirection) const;

	FVector GetAimDirection() const;


	// Ammo 관련 Player에게 전달해야하는 값들이 존재. Getter 생성했습니다. 필요시 해당 Type,CallName,ReturnValue 유지 상태의 리팩토링 부탁드립니다.
public:
	UFUNCTION()
	float GetCurrentAmmo() { return CurrentAmmo; }

	UFUNCTION()
	float GetMaxAmmo() { return MaxAmmo; }
	
//총기의 스텟관련 변수들,assetraw에서 데이터 받아와서저장시킬예정
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
	FVector MuzzleSpot;
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
    float Damage = 10.0f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GunStatus")
    float GunDelay = 0.2f;
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
	UPROPERTY()
	FTimerHandle TestFireTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	EAmmoType AmmoType; 

	//총기발사 화염이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* MuzzleFlashEffect;
};
