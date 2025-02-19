#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimerManager.h"
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

	void DebugTakeDamage();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


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

	void TakeDamage(float DamageAmount);
	void Heal(float HealAmount);
	void StartShieldRegen();
	void RegenerateShield();

private:
	void InitializeCharacter();

	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float DefaultCapsuleHalfHeight;
	bool bIsCrouched = false;

	FVector MeshOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	bool bCanDash = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDistance = 1750.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashCooldown = 0.5f;

	
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
};
