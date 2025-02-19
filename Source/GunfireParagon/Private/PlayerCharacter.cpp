#include "PlayerCharacter.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"


APlayerCharacter::APlayerCharacter()
{
	InitializeCharacter();

	PrimaryActorTick.bCanEverTick = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::InitializeCharacter()
{
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComponent);
	bUseControllerRotationPitch = true;



	NormalSpeed = 800.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(40.0f);

	DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MeshOffset = GetMesh()->GetRelativeLocation();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopSprint
				);
			}

			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::StartCrouch);

				EnhancedInput->BindAction(
					PlayerController->CrouchAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopCrouch);
			}

			if (PlayerController->DashAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DashAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Dash
				);
			}

			if (PlayerController->DebugDamageAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DebugDamageAction,
					ETriggerEvent::Started,
					this,
					&APlayerCharacter::DebugTakeDamage
				);
			}
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}
void APlayerCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}
void APlayerCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}
void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}
void APlayerCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}
void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::StartCrouch(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsMovingOnGround() && !GetCharacterMovement()->IsCrouching())
	{
		GetCharacterMovement()->SetCrouchedHalfHeight(40.0f);
		Crouch();

		FVector NewMeshLocation = MeshOffset;
		NewMeshLocation.Z -= 40.0f;
		GetMesh()->SetRelativeLocation(NewMeshLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void APlayerCharacter::StopCrouch(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
		GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight, true);

		GetMesh()->SetRelativeLocation(MeshOffset);

		FVector TargetLocation = CameraComp->GetRelativeLocation();
		TargetLocation.Z += 40.0f;
		SmoothCameraTransition(TargetLocation, 0.8f);
	}
}


void APlayerCharacter::SmoothCameraTransition(FVector TargetLocation, float Duration)
{
	FVector StartLocation = CameraComp->GetRelativeLocation();
	float ElapsedTime = 0.0f;

	float TimerInterval = 0.02f;
	GetWorldTimerManager().SetTimer(CameraTimer, FTimerDelegate::CreateLambda([this, StartLocation, TargetLocation, Duration, &ElapsedTime]()
		{
			float DeltaTime = GetWorld()->GetDeltaSeconds();
			ElapsedTime += DeltaTime;

			float Alpha = FMath::InterpEaseOut(0.0f, 1.0f, ElapsedTime / Duration, 2.5f);

			FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
			CameraComp->SetRelativeLocation(NewLocation);


			if (Alpha >= 1.0f)
			{
				GetWorldTimerManager().ClearTimer(CameraTimer);
			}
		}), TimerInterval, true);
}



void APlayerCharacter::Dash(const FInputActionValue& value)
{
	if (!bCanDash || !Controller) return;

	bCanDash = false;

	float DashTime = 0.05f;
	float DashSpeed = DashDistance / DashTime;

	
	FVector DashDirection = GetActorForwardVector();
	FVector DashVelocity = DashDirection * DashSpeed;

	LaunchCharacter(DashVelocity, true, true);

	GetWorldTimerManager().SetTimer(
		DashStopTimer,
		this,
		&APlayerCharacter::StopDash,
		DashTime,
		false
	);

	GetWorldTimerManager().SetTimer(
		DashCooldownTimer,
		this,
		&APlayerCharacter::ResetDash,
		DashCooldown,
		false
	);
}

void APlayerCharacter::StopDash()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void APlayerCharacter::ResetDash()
{
	bCanDash = true;
}

void APlayerCharacter::TakeDamage(float DamageAmount)
{
	if (CurrentShield > 0)
	{
		float RemainingDamage = FMath::Clamp(DamageAmount - CurrentShield, 0.0f, DamageAmount);
		CurrentShield = FMath::Clamp(CurrentShield - DamageAmount, 0.0f, MaxShield);

		if (RemainingDamage > 0)
		{
			CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaxHealth);
		}
	}
	else
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	}

	UE_LOG(LogTemp, Warning, TEXT("플레이어가 %f 피해를 받음... 현재 체력: %f, 실드: %f"), DamageAmount, CurrentHealth, CurrentShield);

	GetWorldTimerManager().ClearTimer(ShieldRegenTimer);
	GetWorldTimerManager().SetTimer(ShieldRegenTimer, this, &APlayerCharacter::StartShieldRegen, ShieldRegenDelay, false);
}

void APlayerCharacter::Heal(float HealAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("체력 회복... 현재 체력: %f"), CurrentHealth);
}

void APlayerCharacter::StartShieldRegen()
{
	UE_LOG(LogTemp, Warning, TEXT("실드 회복 시작"));

	if (CurrentShield < MaxShield)
	{
		GetWorldTimerManager().SetTimer(ShieldRegenTimer, this, &APlayerCharacter::RegenerateShield, 1.0f, true);
	}
}

void APlayerCharacter::RegenerateShield()
{
	if (CurrentShield >= MaxShield)
	{
		GetWorldTimerManager().ClearTimer(ShieldRegenTimer);
		UE_LOG(LogTemp, Warning, TEXT("실드가 가득 참!"));
		return;
	}

	CurrentShield = FMath::Clamp(CurrentShield + ShieldRegenRate, 0.0f, MaxShield);
	UE_LOG(LogTemp, Warning, TEXT("실드 회복 중... 현재 실드: %f"), CurrentShield);
}

void APlayerCharacter::DebugTakeDamage()
{
	TakeDamage(20.0f); // 20 데미지를 가함
	UE_LOG(LogTemp, Warning, TEXT("디버그: 플레이어에게 20 데미지를 가했습니다! 현재 체력: %f, 실드: %f"), CurrentHealth, CurrentShield);
}
