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

	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;
	ReturnHPValue();
	ReturnShieldValue();
	if (DefaultWeaponClass)
	{
		ACGunBase* DefaultWeapon = GetWorld()->SpawnActor<ACGunBase>(DefaultWeaponClass);

		if (DefaultWeapon)
		{
			PrimaryWeapon = DefaultWeapon;
			EquipWeapon(PrimaryWeapon, 1);

			UE_LOG(LogTemp, Warning, TEXT("기본 무기 장착 완료: %s"), *PrimaryWeapon->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("기본 무기 스폰 실패"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultWeaponClass가 설정되지 않음"));
	}

	if (PrimaryWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimaryWeapon이 정상적으로 설정됨: %s"), *PrimaryWeapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("게임 시작 후 PrimaryWeapon이 nullptr 상태"));
	}
}



void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentWeapon)
	{
	}
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
			BIND_INPUT_ACTION(CrouchAction, ETriggerEvent::Triggered, StartCrouch);
			BIND_INPUT_ACTION(CrouchAction, ETriggerEvent::Completed, StopCrouch);
			BIND_INPUT_ACTION(DashAction, ETriggerEvent::Triggered, Dash);
			BIND_INPUT_ACTION(FireAction, ETriggerEvent::Triggered, FireWeapon);
			BIND_INPUT_ACTION(SwitchToPrimaryWeaponAction, ETriggerEvent::Triggered, SwitchToPrimaryWeapon);
			BIND_INPUT_ACTION(SwitchToSecondaryWeaponAction, ETriggerEvent::Triggered, SwitchToSecondaryWeapon);
			BIND_INPUT_ACTION(PickupWeaponAction, ETriggerEvent::Triggered, PickupWeaponInput);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("입력 바인딩 완료: G 키가 PickupWeaponInput()에 연결됨"));
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
	}
}


//void APlayerCharacter::SmoothCameraTransition(FVector TargetLocation, float Duration)
//{
//	FVector StartLocation = CameraComp->GetRelativeLocation();
//	float ElapsedTime = 0.0f;
//
//	float TimerInterval = 0.02f;
//	GetWorldTimerManager().SetTimer(CameraTimer, FTimerDelegate::CreateLambda([this, StartLocation, TargetLocation, Duration, &ElapsedTime]()
//		{
//			float DeltaTime = GetWorld()->GetDeltaSeconds();
//			ElapsedTime += DeltaTime;
//
//			float Alpha = FMath::InterpEaseOut(0.0f, 1.0f, ElapsedTime / Duration, 2.5f);
//
//			FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
//			CameraComp->SetRelativeLocation(NewLocation);
//
//
//			if (Alpha >= 1.0f)
//			{
//				GetWorldTimerManager().ClearTimer(CameraTimer);
//			}
//		}), TimerInterval, true);
//}



void APlayerCharacter::Dash(const FInputActionValue& value)
{
	if (!GetWorld())
	{
		return;
	}

	if (!bCanDash || !Controller)
	{
		return;
	}

	bCanDash = false;
	OnDashState.Broadcast(true);
	OnDashCoolDown.Broadcast(DashCooldown);
	float DashTime = 0.1f;
	float DashSpeed = DashDistance / DashTime;

	FVector DashDirection = GetActorForwardVector();
	FVector DashVelocity = DashDirection * DashSpeed;

	LaunchCharacter(DashVelocity, true, true);

	GetWorldTimerManager().SetTimer(DashStopTimer, this, &APlayerCharacter::StopDash, DashTime, false);



	GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &APlayerCharacter::ResetDash, DashCooldown, false);

	if (GetWorldTimerManager().IsTimerActive(DashCooldownTimer))
	{
	}
	else
	{
		ResetDash();
	}
}



void APlayerCharacter::StopDash()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}

	OnDashState.Broadcast(false);
}

void APlayerCharacter::ResetDash()
{
	bCanDash = true;

	if (GetWorldTimerManager().IsTimerActive(DashCooldownTimer))
	{
		GetWorldTimerManager().ClearTimer(DashCooldownTimer);
	}
}


float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
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

	UE_LOG(LogTemp, Warning, TEXT("플레이어가 공격받음 피해량: %f | 현재 체력: %f | 실드: %f"),
		DamageAmount, CurrentHealth, CurrentShield);

	GetWorldTimerManager().ClearTimer(ShieldRegenTimer);
	GetWorldTimerManager().SetTimer(ShieldRegenTimer, this, &APlayerCharacter::StartShieldRegen, ShieldRegenDelay, false);

	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어 사망"));
	}

	return DamageAmount;
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



void APlayerCharacter::SetAmmoState(const float& UpdateCurrentAmmo, const float& UpdateMaxAmmo)
{
	if (IsValid(CurrentWeapon))
	{
		CurrentAmmo = UpdateCurrentAmmo;
		MaxAmmo = UpdateMaxAmmo;

		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}
}

//void APlayerCharacter::SwitchWeapon(ACGunBase* NewWeapon)
//{
//	EquipWeapon(NewWeapon, 2);
//}

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("FireWeapon() 호출됨!"));

	if (CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 무기 Fire() 호출: %s"), *CurrentWeapon->GetName());
		CurrentWeapon->Fire();
		SetAmmoState(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetMaxAmmo());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("현재 장착된 무기가 없음!"));
	}
}




void APlayerCharacter::EquipWeapon(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon 실패: NewWeapon이 NULL임"));
		return;
	}

	FVector WeaponScale = NewWeapon->GetActorScale3D(); // ✅ 현재 스케일 저장

	if (Slot == 1)
	{
		PrimaryWeapon = NewWeapon;
		CurrentWeapon = PrimaryWeapon;
	}
	else if (Slot == 2)
	{
		SecondaryWeapon = NewWeapon;
		CurrentWeapon = SecondaryWeapon;
	}

	AttachWeaponToHand(CurrentWeapon, Slot);

	// ✅ 스케일 유지
	CurrentWeapon->SetActorScale3D(WeaponScale);

	UE_LOG(LogTemp, Warning, TEXT("%d번 슬롯에 무기 장착 완료: %s (스케일: %s)"),
		Slot, *NewWeapon->GetName(), *WeaponScale.ToString());
}







void APlayerCharacter::DropCurrentWeapon(int32 Slot)
{
	ACGunBase*& WeaponSlot = (Slot == 1) ? PrimaryWeapon : SecondaryWeapon;
	if (!WeaponSlot) return;

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FVector WeaponScale = WeaponSlot->GetRootComponent()->GetComponentScale(); // ✅ RootComp의 스케일 가져오기

	WeaponSlot->SetActorLocation(DropLocation);
	WeaponSlot->GetRootComponent()->SetWorldScale3D(WeaponScale); // ✅ RootComp의 스케일 유지

	UE_LOG(LogTemp, Warning, TEXT("%s 무기 드랍됨 (스케일 유지: %s)"), *WeaponSlot->GetName(), *WeaponScale.ToString());

	WeaponSlot->bWasDropped = true;
	WeaponSlot->Tags.AddUnique("DroppedWeapon");

	if (WeaponSlot->WeaponMesh)
	{
		WeaponSlot->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		WeaponSlot->WeaponMesh->SetSimulatePhysics(true);
		WeaponSlot->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponSlot->WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);

		FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, -800.0f);
		WeaponSlot->WeaponMesh->AddImpulse(DropImpulse, NAME_None, true);
	}

	WeaponSlot = nullptr;
}













void APlayerCharacter::AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon || !NewWeapon->GetWeaponMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("AttachWeaponToHand 실패: WeaponMesh가 NULL임!"));
		return;
	}

	FVector WeaponScale = NewWeapon->GetRootComponent()->GetComponentScale(); // ✅ 기존 스케일 저장

	FName HandSocket;
	switch (NewWeapon->WeaponType)
	{
	case EWeaponType::Rifle:
		HandSocket = TEXT("RifleSocket");
		break;
	case EWeaponType::Shotgun:
		HandSocket = TEXT("ShotgunSocket");
		break;
	default:
		HandSocket = TEXT("WeaponSocket");
		break;
	}

	// ✅ 무기 부착
	NewWeapon->GetWeaponMesh()->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HandSocket
	);

	// ✅ RootComp & WeaponMesh 스케일 강제 적용
	NewWeapon->GetRootComponent()->SetWorldScale3D(WeaponScale);
	NewWeapon->GetWeaponMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); // 상대적 스케일 초기화 방지

	NewWeapon->SetActorHiddenInGame(false);
	CurrentWeapon = NewWeapon;

	UE_LOG(LogTemp, Warning, TEXT("무기 장착 완료: %s (소켓: %s, RootComp 스케일: %s)"),
		*NewWeapon->GetName(), *HandSocket.ToString(), *WeaponScale.ToString());
}











void APlayerCharacter::PickupWeaponInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("G 키 입력 감지됨: 무기 줍기 시도"));

	PickupWeapon();
}



void APlayerCharacter::SwapWeaponWithDropped(ACGunBase* DroppedWeapon)
{
	if (!DroppedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("교체할 무기가 없습니다."));
		return;
	}

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	if (CurrentWeaponSlot == 2)
	{
		if (SecondaryWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("기존 2번 무기 드랍: %s"), *SecondaryWeapon->GetName());

			//기존 무기 부착 해제
			SecondaryWeapon->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			//물리 시뮬레이션 활성화
			SecondaryWeapon->WeaponMesh->SetSimulatePhysics(true);
			SecondaryWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SecondaryWeapon->WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);

			//Impulse 적용 (살짝 튕겨나가게)
			FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, -800.0f);
			SecondaryWeapon->WeaponMesh->AddImpulse(DropImpulse, NAME_None, true);

			UE_LOG(LogTemp, Warning, TEXT("기존 무기에 Impulse 적용 완료 : % s"), *SecondaryWeapon->GetName());
		}

		EquipWeapon(DroppedWeapon, 2);
		UE_LOG(LogTemp, Warning, TEXT("2번 무기 교체 완료, 새로운 무기: %s"), *DroppedWeapon->GetName());
	}
}

void APlayerCharacter::PickupWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("무기 줍기 실행됨"));

	ACGunBase* NearbyWeapon = FindNearbyDroppedWeapon();
	if (!NearbyWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("주울 수 있는 무기가 없음"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 획득 성공: %s"), *NearbyWeapon->GetName());

	// ✅ 기존 무기 숨기기 (현재 손에 들고 있는 무기가 있으면)
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("기존 무기 숨김 처리: %s"), *CurrentWeapon->GetName());
	}

	// 2번 슬롯에 장착
	EquipWeapon(NearbyWeapon, 2);
}



ACGunBase* APlayerCharacter::FindNearbyDroppedWeapon()
{
	TArray<AActor*> OverlappingActors;
	float PickupRadius = 1000.0f;

	UKismetSystemLibrary::SphereOverlapActors(
		this, GetActorLocation(), PickupRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
		ACGunBase::StaticClass(), {}, OverlappingActors
	);

	for (AActor* Actor : OverlappingActors)
	{
		ACGunBase* FoundWeapon = Cast<ACGunBase>(Actor);
		if (FoundWeapon)
		{
			FVector WeaponScale = FoundWeapon->GetActorScale3D();
			UE_LOG(LogTemp, Warning, TEXT("감지된 무기: %s | 현재 스케일: %s"),
				*FoundWeapon->GetName(), *WeaponScale.ToString());

			return FoundWeapon;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("주울 수 있는 무기가 없음"));
	return nullptr;
}






void APlayerCharacter::SwitchWeaponSlot(int32 Slot)
{
	if (Slot == 1)
	{
		if (!PrimaryWeapon)
		{
			UE_LOG(LogTemp, Error, TEXT("1번 무기가 없습니다. PrimaryWeapon이 nullptr 상태"));
			return;
		}

		// 기존 무기 숨김 처리
		if (CurrentWeapon && CurrentWeapon != PrimaryWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			UE_LOG(LogTemp, Warning, TEXT("기존 무기 숨김 처리: %s"), *CurrentWeapon->GetName());
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

		// 기존 무기 숨김 처리
		if (CurrentWeapon && CurrentWeapon != SecondaryWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			UE_LOG(LogTemp, Warning, TEXT("기존 무기 숨김 처리: %s"), *CurrentWeapon->GetName());
		}

		CurrentWeapon = SecondaryWeapon;
		CurrentWeaponSlot = 2;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("잘못된 슬롯 번호: %d"), Slot);
		return;
	}

	AttachWeaponToHand(CurrentWeapon, Slot);
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