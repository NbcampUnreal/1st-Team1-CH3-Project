#pragma once

#include "CoreMinimal.h"
#include "FPSDataTables.h"
#include "GameFramework/Actor.h"
#include "Actor/Bullet/BulletBase.h"
#include "NiagaraComponent.h"
#include "Sound/SoundBase.h"
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
	~ACGunBase();
	
	virtual void Fire();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunMesh")
	UStaticMeshComponent* GunMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;
	//탄창을 재장전
	virtual void Reload();

	AFPSDataTables* DataTables;
	// 무기데이터 키값을 받아서 데이터 재정립하는 함수
	void SetWeaponData(float Weaponkey);

	bool bISDrop = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	int32 WeaponDataKey = 1001;

	void SetIsDrop(bool isDrop);

	
	//총기 딜레이에 따라 발사 가능 여부 확인
	bool CanFire()const ;
	//나중에 바인딩할때 마우스 떼면 그만 쏘도록
	void StopFire();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootComp;


protected:
	virtual void BeginPlay() override;


	
	void SetIsFire(); 
	//탄창이 비었는지 확인
	bool IsAmmoEmpty()const ;
	void SwitchGunSound(EWeaponType WeaponClass);

	
	
	FVector SpreadDirection(const FVector OriginDirection) const;

	FVector GetAimDirection() const;


	// Ammo 관련 Player에게 전달해야하는 값들이 존재. Getter 생성했습니다. 필요시 해당 Type,CallName,ReturnValue 유지 상태의 리팩토링 부탁드립니다.
public:
	
	UFUNCTION()
	float GetCurrentAmmo()const { return CurrentAmmo; }
	
	UFUNCTION()
	float GetMaxAmmo()const { return MaxAmmo; }
	
	UFUNCTION(BlueprintCallable)
	void DisableWeaponShadows();
	
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
protected:
	
	//드롭이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DropEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* DropEffectComp;
	

	//총기사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Sounds")
	TSoftObjectPtr<USoundBase> FireSound;

public:
	float GetGunDelay()const { return GunDelay; }
	void SetGunDelay(float amount){ GunDelay = amount; };
	float GetGunDamage()const { return Damage; }
	void SetGunDamage(float amount){ Damage = amount; };
	void SetMaxAmmo(int32 amount) { MaxAmmo = amount; };


};