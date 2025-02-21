#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "CGunBase.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
struct FInputActionValue;

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
	


	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StartCrouch(const FInputActionValue& Value);
	UFUNCTION()
	void StopCrouch(const FInputActionValue& Value);
	UFUNCTION()
	void Dash(const FInputActionValue& Value);
	UFUNCTION()
	void FireWeapon(const FInputActionValue& Value);

	UFUNCTION()
	void HandleSwapWeaponInput(const FInputActionValue& Value);
	UFUNCTION()
	void EquipWeapon(ACGunBase* NewWeapon, int32 Slot);
	void TakeDamage(float DamageAmount);
	void Heal(float HealAmount);
	void StartShieldRegen();
	void RegenerateShield();
	//무기 교체 함수
	void SwitchWeapon(ACGunBase* NewWeapon);
	void SwapWeaponWithDropped(ACGunBase* NewWeapon);
	virtual void Landed(const FHitResult& Hit) override;


private:
	void InitializeCharacter();

	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float DefaultCapsuleHalfHeight;
	bool bIsCrouched = false;
	int32 JumpCount = 0;
	int32 MaxJumpCount = 2;

	FVector MeshOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	bool bCanDash = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDistance = 1750.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashCooldown = 0.5f;

	//1번 슬롯 (기본 무기 포함, 교체 가능)
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* PrimaryWeapon = nullptr;
	//2번 슬롯 (교체 가능)
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ACGunBase* SecondaryWeapon = nullptr;
	//현재 들고 있는 무기 (1번 또는 2번)
	ACGunBase* CurrentWeapon = nullptr;
    int32 CurrentWeaponSlot = 1;
	//무기 소켓
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FName WeaponSocketName = "WeaponSocket";
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector RifleOffset = FVector(-60.0f, 22.0f, 0.0f);
	UPROPERTY(EditAnywhere, Category = "Weapon")
	FVector PistolOffset = FVector(-5.0f, 2.0f, 0.0f);
	void AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot);
	void DropCurrentWeapon(int32 Slot);


	FTimerHandle DashCooldownTimer;
	FTimerHandle DashStopTimer;
	FTimerHandle CameraTimer;
	FTimerHandle ShieldRegenTimer;

	UFUNCTION()
	void ResetDash();
	UFUNCTION()
	void StopDash();
	UFUNCTION()
	void SmoothCameraTransition(FVector TargetLocation, float Duration);
	ACGunBase* FindNearbyDroppedWeapon();

public:
	void DebugTakeDamage();
};
