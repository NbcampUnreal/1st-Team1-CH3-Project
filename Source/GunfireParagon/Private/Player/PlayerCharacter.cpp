#include "Player/PlayerCharacter.h"
#include "Player/MyPlayerController.h"
#include "Actor/Weapon/CGunBase.h"
#include "Actor/Weapon/Gun_Rifle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


APlayerCharacter::APlayerCharacter()
{
	InitializeCharacter();

	PrimaryActorTick.bCanEverTick = false;

	DefaultWeaponClass = AGun_Rifle::StaticClass();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;

	if (DefaultWeaponClass)
	{
		ACGunBase* DefaultWeapon = GetWorld()->SpawnActor<ACGunBase>(DefaultWeaponClass);
		if (DefaultWeapon)
		{
			EquipWeapon(DefaultWeapon, 1);
			UE_LOG(LogTemp, Warning, TEXT("기본 라이플 장착 완료: %s"), *DefaultWeapon->GetName());
		}
	}
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

#define BIND_INPUT_ACTION(Action, Event, Function) \
    if (PlayerController->Action) \
    { \
        EnhancedInput->BindAction(PlayerController->Action, Event, this, &APlayerCharacter::Function); \
    }

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			BIND_INPUT_ACTION(MoveAction, ETriggerEvent::Triggered, Move);
			BIND_INPUT_ACTION(LookAction, ETriggerEvent::Triggered, Look);
			BIND_INPUT_ACTION(JumpAction, ETriggerEvent::Started, StartJump);
			BIND_INPUT_ACTION(JumpAction, ETriggerEvent::Completed, StopJump);
			BIND_INPUT_ACTION(SprintAction, ETriggerEvent::Triggered, StartSprint);
			BIND_INPUT_ACTION(SprintAction, ETriggerEvent::Completed, StopSprint);
			BIND_INPUT_ACTION(FireAction, ETriggerEvent::Triggered, FireWeapon);
			BIND_INPUT_ACTION(SwitchToPrimaryWeaponAction, ETriggerEvent::Triggered, SwitchToPrimaryWeapon);
			BIND_INPUT_ACTION(SwitchToSecondaryWeaponAction, ETriggerEvent::Triggered, SwitchToSecondaryWeapon);
			BIND_INPUT_ACTION(SwapWeaponAction, ETriggerEvent::Triggered, HandleSwapWeaponInput);
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
	if (JumpCount < MaxJumpCount)
	{
		Jump();
		JumpCount++;
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
	if (!bCanDash || !Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ 대시 불가능: bCanDash = %d, Controller = %p"), bCanDash, Controller.Get());
		return;
	}

	bCanDash = false;
	UE_LOG(LogTemp, Warning, TEXT("✅ 대시 시작!"));

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
		UE_LOG(LogTemp, Warning, TEXT("실드가 가득 참"));
		return;
	}

	CurrentShield = FMath::Clamp(CurrentShield + ShieldRegenRate, 0.0f, MaxShield);
	UE_LOG(LogTemp, Warning, TEXT("실드 회복 중... 현재 실드: %f"), CurrentShield);
}

void APlayerCharacter::DebugTakeDamage()
{
	TakeDamage(20.0f);
	UE_LOG(LogTemp, Warning, TEXT("플레이어에게 20 데미지를 가했습니다 현재 체력: %f, 실드: %f"), CurrentHealth, CurrentShield);
}

void APlayerCharacter::SwitchWeapon(ACGunBase* NewWeapon)
{
	EquipWeapon(NewWeapon, 2);
}

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 장착된 무기가 없습니다."));
		return;
	}

	CurrentWeapon->Fire();
}



void APlayerCharacter::EquipWeapon(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon) return;

	DropCurrentWeapon(Slot);  // 기존 무기 드랍
	AttachWeaponToHand(NewWeapon, Slot); // 새 무기 장착
}

void APlayerCharacter::DropCurrentWeapon(int32 Slot)
{
	ACGunBase*& WeaponSlot = (Slot == 1) ? PrimaryWeapon : SecondaryWeapon;
	if (!WeaponSlot) return;

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	WeaponSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WeaponSlot->SetActorLocation(DropLocation);
	WeaponSlot->SetActorHiddenInGame(false);
	WeaponSlot->SetActorEnableCollision(true);

	if (WeaponSlot->GunMesh)
	{
		WeaponSlot->GunMesh->SetSimulatePhysics(true);
		WeaponSlot->GunMesh->AddImpulse(FVector(0, 0, -300.0f), NAME_None, true);
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 드랍 완료: %s"), *WeaponSlot->GetName());
	WeaponSlot = nullptr;
}


void APlayerCharacter::AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon) return;

	ACGunBase*& WeaponSlot = (Slot == 1) ? PrimaryWeapon : SecondaryWeapon;
	WeaponSlot = NewWeapon;
	CurrentWeapon = WeaponSlot;
	CurrentWeaponSlot = Slot;

	NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s (슬롯: %d)"), *NewWeapon->GetName(), Slot);
}












void APlayerCharacter::HandleSwapWeaponInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("G키 입력 감지됨: 무기 교체 시도 중"));

	ACGunBase* DroppedWeapon = FindNearbyDroppedWeapon();
	if (!DroppedWeapon)
	{
		return;
	}

	if (!SecondaryWeapon)
	{
		EquipWeapon(DroppedWeapon, 2);
		UE_LOG(LogTemp, Warning, TEXT("2번 슬롯이 비어 있어 자동 장착됨: %s"), *DroppedWeapon->GetName());
		return;
	}

	SwapWeaponWithDropped(DroppedWeapon);
}

void APlayerCharacter::SwapWeaponWithDropped(ACGunBase* DroppedWeapon)
{
	if (!DroppedWeapon)
	{
		return;
	}

	if (CurrentWeaponSlot == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("기본 무기는 교체할 수 없습니다"));
		return;
	}

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	if (CurrentWeaponSlot == 2)
	{
		if (SecondaryWeapon)
		{
			SecondaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			SecondaryWeapon->SetActorLocation(DropLocation);

			if (SecondaryWeapon->WeaponMesh)
			{
				SecondaryWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SecondaryWeapon->WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);
				SecondaryWeapon->WeaponMesh->SetSimulatePhysics(true);
				SecondaryWeapon->WeaponMesh->AddImpulse(FVector(0, 0, -300.0f), NAME_None, true);
			}

			SecondaryWeapon->SetActorHiddenInGame(false);
			SecondaryWeapon->SetActorEnableCollision(true);
			SecondaryWeapon->bWasDropped = true;

			UE_LOG(LogTemp, Warning, TEXT("2번 무기 드랍 완료 위치: %s"), *DropLocation.ToString());
		}

		EquipWeapon(DroppedWeapon, 2);
		UE_LOG(LogTemp, Warning, TEXT("2번 무기 교체 완료 새로운 무기: %s"), *DroppedWeapon->GetName());
	}
}





ACGunBase* APlayerCharacter::FindNearbyDroppedWeapon()
{
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::SphereOverlapActors(
		this, GetActorLocation(), 600.0f,
		{ EObjectTypeQuery::ObjectTypeQuery3 },  // 불필요한 ObjectTypeQuery 제거
		ACGunBase::StaticClass(), {}, OverlappingActors
	);

	ACGunBase* ClosestWeapon = nullptr;
	float ClosestDistSq = FLT_MAX;

	for (AActor* Actor : OverlappingActors)
	{
		ACGunBase* FoundWeapon = Cast<ACGunBase>(Actor);
		if (!FoundWeapon) continue;

		float DistSq = FVector::DistSquared(GetActorLocation(), FoundWeapon->GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestWeapon = FoundWeapon;
		}
	}

	return ClosestWeapon;
}










void APlayerCharacter::SwitchWeaponSlot(int32 Slot)
{
	if (Slot == 1)
	{
		if (!PrimaryWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("1번 무기가 없습니다."));
			return;
		}

		CurrentWeapon = PrimaryWeapon;
		CurrentWeaponSlot = 1;
	}
	else if (Slot == 2)
	{
		if (!SecondaryWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("2번 무기가 없습니다."));
			return;
		}

		CurrentWeapon = SecondaryWeapon;
		CurrentWeaponSlot = 2;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("잘못된 슬롯 번호: %d"), Slot);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 슬롯 변경 완료. 현재 무기: %s"), *CurrentWeapon->GetName());
}


void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	JumpCount = 0;
}

void APlayerCharacter::SwitchToPrimaryWeapon()
{
	SwitchWeaponSlot(1);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	SwitchWeaponSlot(2);
}




