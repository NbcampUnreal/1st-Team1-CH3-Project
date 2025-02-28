#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "TimerManager.h"
#include "Actor/Weapon/CGunBase.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedPlayerValue, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeOneValue, float, OneValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeBoolValue, bool, OneValue);

UCLASS()
class GUNFIREPARAGON_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsHoldingRifle = false;

	void SwitchWeaponSlot(int32 Slot);
	void SwitchToPrimaryWeapon();
	void SwitchToSecondaryWeapon();
	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACGunBase> DefaultWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxShield = 50.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ShieldRegenRate = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float ShieldRegenDelay = 3.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 1.0f;

	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	/*UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);*/
	UFUNCTION()
	void Dash(const FInputActionValue& Value);
	UFUNCTION()
	void FireWeapon(const FInputActionValue& Value);
	UFUNCTION()
	void PickupWeapon();
	UFUNCTION()
	void PickupWeaponInput(const FInputActionValue& Value);
	UFUNCTION()
	bool EquipWeapon(ACGunBase* NewWeapon, int32 Slot);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetMouseSensitivity(float NewSensitivity);
	UFUNCTION()
	void IncreaseMouseSensitivity();
	UFUNCTION()
	void DecreaseMouseSensitivity();


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	void Heal(float HealAmount);
	void StartShieldRegen();
	void RegenerateShield();
	void SwapWeaponWithDropped(ACGunBase* NewWeapon);
	virtual void Landed(const FHitResult& Hit) override;

private:
	void InitializeCharacter();

	ACGunBase* Inventory[2];
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float DefaultCapsuleHalfHeight;
	int32 JumpCount = 0;
	int32 MaxJumpCount = 2;

	FVector MeshOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	bool bCanDash = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDistance = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashCooldown = 1.5f;
	bool bCanSwitchWeapon = true;
	UPROPERTY()
	TArray<ACGunBase*> DroppedWeapons;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* PrimaryWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* SecondaryWeapon = nullptr;
	ACGunBase* CurrentWeapon = nullptr;
    int32 CurrentWeaponSlot = 1;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName = "WeaponSocket";
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector RifleOffset = FVector(-60.0f, 22.0f, 0.0f);
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector PistolOffset = FVector(-5.0f, 2.0f, 0.0f);
	void AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot);
	void DropCurrentWeapon(int32 Slot);
	bool bCanPickupWeapon = true;
	FTimerHandle PickupCooldownTimer;
	void ResetPickupWeapon();
	bool bCanDropWeapon = true; 
	FTimerHandle DropWeaponCooldownTimer; 
	void ResetDropWeaponCooldown();

	FTimerHandle DashCooldownTimer;
	FTimerHandle DashStopTimer;
	FTimerHandle CameraTimer;
	FTimerHandle ShieldRegenTimer;

	UFUNCTION()
	void ResetDash();
	UFUNCTION()
	void StopDash();
	ACGunBase* FindNearbyDroppedWeapon();

// Event Binding To WBP - KGW
// 병합시 충돌 관련해서 문제 발생시 해당 과정 추가 바랍니다.
// 해당 Value관련 델리게이트 Broad 전달 경우, 재사용성을 위해 델리게이트 선언 부 DYNAMIC_MULTICAST 형태로 선언하였으니 필요시 BroadCast하여 전달하셔도 괜찮습니다.
public:
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangeBoolValue OnDashState;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangeOneValue OnDashCoolDown;

	// Ammo 관련 변수/함수 존재하지 않음. 플레이어 캐릭터 내 추가 바랍니다. 해당 형태를 Helath선언부분으로 올리셔도 괜찮습니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile")
	float CurrentAmmo = 0;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile")
	float MaxAmmo = 0;
	void SetAmmoState(const float& UpdateCurrentAmmo, const float& UpdateMaxAmmo);
	void HideCurrentWeapon();

	void ReturnHPValue()
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}

	void ReturnShieldValue()
	{
		OnShieldChanged.Broadcast(CurrentShield, MaxShield);
	}
};

/*  UI-> Player Comment
	BeginPlay 초기 Status값 관련 하나의 SetFunction을 사용하여 정리해주셨으면 합니다.
	현재 Status Value 변경 시 각각의 함수 내에서 처리되기때문에 유지보수성이 너무 낮습니다.
	해당 근거를 바탕으로 Damege처리, ShieldRegen, Level/CardSelect로 인한 Statue 변화의 경우 
	SetterHP/ShieldValue Func 사용하여 응집도를 높여주시며, 결합성을 낮춰주셨으면합니다.
	Rebase And Merge 브랜치 전략을 사용함으로써 PR을 지양하는 방향성 이기 때문에 Comment 남깁니다.
*/