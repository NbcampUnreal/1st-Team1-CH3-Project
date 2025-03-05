#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "GameMode/FPSGameInstance.h"
#include "GameMode/FPSGameMode.h"
#include "GameMode/CardData.h"
#include "TimerManager.h"
#include "Actor/Weapon/CGunBase.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedPlayerValue, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeOneValue, float, OneValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeBoolValue, bool, OneValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeCurrentWeapon, ACGunBase*, OneValue);

UCLASS()
class GUNFIREPARAGON_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsHoldingRifle = false;

	UFUNCTION()
	void SetPlayerStatus(float NewHealth, float NewShield);
	UFUNCTION()
	void GainExperience(float ExpAmount);
	UFUNCTION(BlueprintCallable)
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetMouseSensitivity(float NewSensitivity);

	void SwitchWeaponSlot(int32 Slot);
	void SwitchToPrimaryWeapon();
	void SwitchToSecondaryWeapon();
	bool IsPlayerDead() { return bIsDead; }
	UPROPERTY()
	TArray<FCardEffect> AppliedCardEffects;
	void ApplyCardEffect(const FCardEffect& SelectedCard);	// (UCardData* SelectedCard);
	void ApplyEffectToGun(FCardEffect Effect);
	ACGunBase* GetEquippedGun();
	ACGunBase* Inventory[2] = { nullptr, nullptr };
	void AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot);
	ACGunBase* CurrentWeapon = nullptr;
	int32 CurrentWeaponSlot = 1;
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void HideFirstPersonMeshAndWeapon();
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void ShowFirstPersonMeshAndWeapon();
	UFUNCTION(BlueprintCallable)
	void SwitchToDeathCamera();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadWeapon();
	virtual void Jump() override;
	void EnableMouseControl();
	void PlayReloadAnimation();
	

protected:
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;
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
	float ShieldRegenDelay = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* ThirdPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* JumpSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ReloadSound;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
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
	UFUNCTION()
	void IncreaseMouseSensitivity();
	UFUNCTION()
	void DecreaseMouseSensitivity();
	
	/*UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);*/
	UFUNCTION(BlueprintCallable, Category = "Player")
	void HandlePlayerDeath();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	void Heal(float HealAmount);
	void StartShieldRegen();
	void RegenerateShield();
	void SwapWeaponWithDropped(ACGunBase* NewWeapon);

	virtual void Landed(const FHitResult& Hit) override;
	void DisableFirstPersonShadows();
	bool bCanSwitchWeapon = true;
	FTimerHandle SwitchWeaponCooldownTimer;
	void ResetWeaponSwitchCooldown();
	void ResetReload();
	bool bCanReload = true;
	FTimerHandle ReloadTimerHandle;

private:
	void InitializeCharacter();
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
	float DashCoolDown = 1.5f;

	UPROPERTY()
	TArray<ACGunBase*> DroppedWeapons;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* PrimaryWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* SecondaryWeapon = nullptr;
	

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
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* DeathCameraSpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* DeathCamera;


public:
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnShieldChanged;
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangedPlayerValue OnAmmoChanged;
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangeCurrentWeapon OnWeaponClass;

	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangeBoolValue OnDashState;
	UPROPERTY(BlueprintAssignable, Category = "UI Events")
	FOnChangeOneValue OnDashCoolDown;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile")
	float CurrentAmmo = 0;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile")
	float MaxAmmo = 0;

	float GetCurrentHealth() { return CurrentHealth; }
	float GetMaxHealth() { return MaxHealth; }
	float GetCurrentShield() { return CurrentShield; }
	float GetMaxShield() { return MaxShield; }
	void SetMaxHealth(float amount) { MaxHealth = amount;}
	void SetMaxShield(float amount) { MaxShield = amount;}
	ACGunBase* GetCurrentWeaponClass() { return CurrentWeapon;}
	int32 GetCurrentWeaponSlot() { return CurrentWeaponSlot;}
	void SetAmmoState(const float& UpdateCurrentAmmo, const float& UpdateMaxAmmo);

	void SetCurrentWeaponClass()
	{
		if (!IsValid(CurrentWeapon)) return;

		OnWeaponClass.Broadcast(CurrentWeapon);
	}

	void HideCurrentWeapon();
};
