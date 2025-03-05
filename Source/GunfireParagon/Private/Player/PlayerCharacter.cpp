#include "Player/PlayerCharacter.h"
#include "Player/MyPlayerController.h"
#include "Actor/Weapon/CGunBase.h"
#include "Actor/Weapon/Gun_Rifle.h"
#include "Actor/Weapon/Gun_Shotgun.h"
#include "Actor/Weapon/Gun_Rocket.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
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

	Tags.Add("Player");

	bIsDead = false;

	DeathCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCameraSpringArm"));
	DeathCameraSpringArm->SetupAttachment(GetCapsuleComponent());
	DeathCameraSpringArm->TargetArmLength = 500.0f;
	DeathCameraSpringArm->bUsePawnControlRotation = false;
	DeathCameraSpringArm->bEnableCameraLag = true;
	DeathCameraSpringArm->CameraLagSpeed = 5.0f;
	DeathCameraSpringArm->SetRelativeLocation(FVector(-200.0f, 0.0f, 100.0f));
	DeathCameraSpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	DeathCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera"));
	DeathCamera->SetupAttachment(DeathCameraSpringArm);
	DeathCamera->FieldOfView = 100.0f;
	DeathCamera->SetAutoActivate(false);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() - 초기 bIsDead 값: %s"), bIsDead ? TEXT("true") : TEXT("false"));

	if (GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("1인칭 메시 확인됨"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("1인칭 메시가 nullptr입니다!"));
	}

	if (ThirdPersonMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("3인칭 메시 확인됨"));
		ThirdPersonMesh->SetOwnerNoSee(false);
		ThirdPersonMesh->bCastDynamicShadow = true;
		ThirdPersonMesh->bCastHiddenShadow = true;
		ThirdPersonMesh->CastShadow = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("3인칭 메시가 nullptr입니다!"));
	}

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	SetPlayerStatus(MaxHealth, MaxShield);

	NormalSpeed = 1000.0f;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadPlayerStats();
		GameInstance->LoadWeaponStats(this);
		GameInstance->LoadMouseSensitivity();
	}

	if (Inventory[0]==nullptr)
	{
		if (DefaultWeaponClass)
		{
			ACGunBase* DefaultWeapon = GetWorld()->SpawnActor<ACGunBase>(DefaultWeaponClass);

			if (DefaultWeapon)
			{
				Inventory[0] = DefaultWeapon;
				CurrentWeapon = Inventory[0];

				CurrentWeaponSlot = 0;

				AttachWeaponToHand(CurrentWeapon, 0);
				CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
				MaxAmmo = CurrentWeapon->GetMaxAmmo();
				UE_LOG(LogTemp, Warning, TEXT("기본 무기 장착 완료: %s"), *Inventory[0]->GetName());
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

	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	MeshOffset = GetMesh()->GetRelativeLocation();

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	if (!ThirdPersonMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeCharacter() - ThirdPersonMesh 생성 실패"));
	}
	ThirdPersonMesh->SetupAttachment(RootComponent);
	ThirdPersonMesh->SetOwnerNoSee(true);
	ThirdPersonMesh->bCastHiddenShadow = true;
	ThirdPersonMesh->CastShadow = true;
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
			BIND_INPUT_ACTION(DashAction, ETriggerEvent::Triggered, Dash);
			BIND_INPUT_ACTION(FireAction, ETriggerEvent::Triggered, FireWeapon);
			BIND_INPUT_ACTION(ReloadAction, ETriggerEvent::Triggered, ReloadWeapon);
			BIND_INPUT_ACTION(SwitchToPrimaryWeaponAction, ETriggerEvent::Triggered, SwitchToPrimaryWeapon);
			BIND_INPUT_ACTION(SwitchToSecondaryWeaponAction, ETriggerEvent::Triggered, SwitchToSecondaryWeapon);
			BIND_INPUT_ACTION(PickupWeaponAction, ETriggerEvent::Triggered, PickupWeaponInput);
			BIND_INPUT_ACTION(IncreaseSensitivityAction, ETriggerEvent::Triggered, IncreaseMouseSensitivity);
			BIND_INPUT_ACTION(DecreaseSensitivityAction, ETriggerEvent::Triggered, DecreaseMouseSensitivity);
			//BIND_INPUT_ACTION(SprintAction, ETriggerEvent::Triggered, StartSprint);
			//BIND_INPUT_ACTION(SprintAction, ETriggerEvent::Completed, StopSprint);
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
		FRotator ControlRotation = Controller->GetControlRotation();
		ControlRotation.Pitch = 0.0f; 
		FVector ForwardDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		FVector RightDirection = GetActorRightVector();
		AddMovementInput(RightDirection, MoveInput.Y);
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

	AddControllerYawInput(LookInput.X * MouseSensitivity);
	AddControllerPitchInput(LookInput.Y * MouseSensitivity);
}

//void APlayerCharacter::StartSprint(const FInputActionValue& value)
//{
//	if (GetCharacterMovement())
//	{
//		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
//	}
//}
//void APlayerCharacter::StopSprint(const FInputActionValue& value)
//{
//	if (GetCharacterMovement())
//	{
//		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
//	}
//}

void APlayerCharacter::Dash(const FInputActionValue& value)
{
	if (!GetWorld() || !bCanDash || !Controller)
	{
		return;
	}

	bCanDash = false;
	OnDashState.Broadcast(true);
	OnDashCoolDown.Broadcast(DashCoolDown);
	float DashTime = 0.1f;
	float DashSpeed = DashDistance / DashTime;

	FVector DashDirection = GetLastMovementInputVector();

	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	DashDirection.Normalize();

	FVector DashVelocity = DashDirection * DashSpeed;
	LaunchCharacter(DashVelocity, true, true);

	GetWorldTimerManager().SetTimer(DashStopTimer, this, &APlayerCharacter::StopDash, DashTime, false);

	GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &APlayerCharacter::ResetDash, DashCoolDown, false);

	UE_LOG(LogTemp, Warning, TEXT("대시 실행: 방향 = %s, 속도 = %f"), *DashDirection.ToString(), DashSpeed);
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

	UE_LOG(LogTemp, Warning, TEXT("대시 쿨다운 종료, 다시 대시 가능"));
}

void APlayerCharacter::IncreaseMouseSensitivity()
{
	SetMouseSensitivity(MouseSensitivity + 0.005f);
}

void APlayerCharacter::DecreaseMouseSensitivity()
{
	SetMouseSensitivity(MouseSensitivity - 0.005f);
}



float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage 호출됨 - 하지만 이미 사망한 상태임 (bIsDead = true)"));
		return 0.0f;
	}

	float RemainingDamage = DamageAmount;
	if (CurrentShield > 0)
	{
		RemainingDamage = FMath::Clamp(DamageAmount - CurrentShield, 0.0f, DamageAmount);
		CurrentShield = FMath::Clamp(CurrentShield - DamageAmount, 0.0f, MaxShield);
	}

	if (RemainingDamage > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaxHealth);
	}

	UE_LOG(LogTemp, Warning, TEXT("플레이어가 공격받음 피해량: %f | 현재 체력: %f | 실드: %f"),
		DamageAmount, CurrentHealth, CurrentShield);

	GetWorldTimerManager().ClearTimer(ShieldRegenTimer);
	GetWorldTimerManager().SetTimer(ShieldRegenTimer, this, &APlayerCharacter::StartShieldRegen, ShieldRegenDelay, false);

	SetPlayerStatus(CurrentHealth, CurrentShield);

	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("체력이 0 이하가 됨 - 사망 처리 시작"));

		if (!bIsDead)
		{
			UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				GameInstance->SavePlayerStats(this);
			}

			HandlePlayerDeath();
			UE_LOG(LogTemp, Warning, TEXT("HandlePlayerDeath() 호출 완료"));
		}
	}

	return DamageAmount;
}



void APlayerCharacter::Heal(float HealAmount)
{
	SetPlayerStatus(CurrentHealth + HealAmount, CurrentShield);
}


void APlayerCharacter::GainExperience(float ExpAmount)
{
	UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AddExperiencePoint(ExpAmount);
	}
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
	OnShieldChanged.Broadcast(CurrentShield, MaxShield);
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

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
		SetAmmoState(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetMaxAmmo());
	}
}


bool APlayerCharacter::EquipWeapon(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon 실패: NewWeapon이 NULL임"));
		return false;
	}

	if (Slot == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(" EquipWeapon 오류 - 1번 무기는 변경할 수 없음!"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT(" EquipWeapon 실행 - 교체 대상: %s"), *NewWeapon->GetName());

	if (CurrentWeapon && CurrentWeapon == Inventory[0])
	{
		UE_LOG(LogTemp, Warning, TEXT("기본 무기 숨김 처리: %s"), *Inventory[0]->GetName());
		Inventory[0]->SetActorHiddenInGame(true);
		Inventory[0]->GetWeaponMesh()->SetVisibility(false);
	}

	if (Inventory[1])
	{
		UE_LOG(LogTemp, Warning, TEXT("기존 무기 드랍 실행: %s"), *Inventory[1]->GetName());
		DropCurrentWeapon(1);
	}

	Inventory[1] = NewWeapon;
	CurrentWeapon = Inventory[1];

	CurrentWeaponSlot = 1;

	AttachWeaponToHand(CurrentWeapon, 1);

	for (const FCardEffect& Effect : AppliedCardEffects)
	{
		ApplyEffectToGun(Effect);
	}

	CurrentWeapon->DisableWeaponShadows();

	return true;
}






void APlayerCharacter::DropCurrentWeapon(int32 Slot)
{
	if (Slot == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("기본 무기(1번 슬롯)는 드랍할 수 없습니다."));
		return;
	}

	if (!bCanDropWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropCurrentWeapon 실행 방지: 쿨다운 중"));
		return;
	}

	ACGunBase*& WeaponSlot = Inventory[1];
	if (!WeaponSlot)
	{
		return;
	}

	bCanDropWeapon = false;
	GetWorldTimerManager().SetTimer(DropWeaponCooldownTimer, this, &APlayerCharacter::ResetDropWeaponCooldown, 0.5f, false);

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	FRotator DropRotation = GetActorRotation();

	if (WeaponSlot->GetWeaponMesh())
	{
		WeaponSlot->GetWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		WeaponSlot->GetWeaponMesh()->SetSimulatePhysics(true);
		WeaponSlot->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	WeaponSlot->SetActorLocation(DropLocation);
	WeaponSlot->SetActorRotation(DropRotation);
	WeaponSlot->SetActorHiddenInGame(false);
	WeaponSlot->SetActorEnableCollision(true);
	WeaponSlot->SetIsDrop(true);

	FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, 200.0f);
	WeaponSlot->GetWeaponMesh()->AddImpulse(DropImpulse, NAME_None, true);

	DroppedWeapons.Add(WeaponSlot);

	Inventory[1] = nullptr;
}


void APlayerCharacter::AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon || !NewWeapon->GetWeaponMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("AttachWeaponToHand 실패: WeaponMesh가 NULL임!"));
		return;
	}

	HideCurrentWeapon();

	FName HandSocket = TEXT("WeaponSocket");
	switch (NewWeapon->WeaponType)
	{
	case EWeaponType::Rifle: HandSocket = TEXT("RifleSocket"); break;
	case EWeaponType::Shotgun: HandSocket = TEXT("ShotgunSocket"); break;
	case EWeaponType::Sniper: HandSocket = TEXT("SniperSocket"); break;
	case EWeaponType::Rocket: HandSocket = TEXT("RocketSocket"); break;
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 장착 시도: %s"), *NewWeapon->GetName());

	NewWeapon->GetWeaponMesh()->SetSimulatePhysics(false);
	NewWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewWeapon->GetWeaponMesh()->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HandSocket
	);

	NewWeapon->SetActorHiddenInGame(false);
	NewWeapon->GetWeaponMesh()->SetVisibility(true);

	CurrentWeapon = NewWeapon;
	CurrentWeaponSlot = Slot;
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

			SecondaryWeapon->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		}

		EquipWeapon(DroppedWeapon, 2);
		UE_LOG(LogTemp, Warning, TEXT("2번 무기 교체 완료, 새로운 무기: %s"), *DroppedWeapon->GetName());

		if (SecondaryWeapon)
		{
			SecondaryWeapon->SetIsDrop(true);
			SecondaryWeapon->WeaponMesh->SetSimulatePhysics(true);
			SecondaryWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SecondaryWeapon->WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);

			FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, -800.0f);
			SecondaryWeapon->WeaponMesh->AddImpulse(DropImpulse, NAME_None, true);

			UE_LOG(LogTemp, Warning, TEXT("기존 무기에 Impulse 적용 완료: %s"), *SecondaryWeapon->GetName());
		}
	}
}



void APlayerCharacter::PickupWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("G 키 입력 감지됨: 무기 줍기 시도"));

	if (!bCanPickupWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("G 키 연속 입력 방지: 무기 줍기 동작이 쿨다운 중입니다!"));
		return;
	}

	bCanPickupWeapon = false;
	GetWorldTimerManager().SetTimer(PickupCooldownTimer, this, &APlayerCharacter::ResetPickupWeapon, 1.0f, false);

	ACGunBase* NearbyWeapon = FindNearbyDroppedWeapon();

	if (!NearbyWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("주울 수 있는 무기가 없음 → 무기 교체 불가"));
		return;
	}

	if ((Inventory[0] && Inventory[0]->WeaponType == NearbyWeapon->WeaponType) ||
		(Inventory[1] && Inventory[1]->WeaponType == NearbyWeapon->WeaponType))
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 같은 타입의 무기를 소지하고 있음: %d → 줍기 불가"), (int32)NearbyWeapon->WeaponType);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 획득 성공: %s"), *NearbyWeapon->GetName());

	ACGunBase* OldWeapon = Inventory[1];

	if (EquipWeapon(NearbyWeapon, 1))
	{
		if (OldWeapon)
		{
			DropCurrentWeapon(1);
		}
		UE_LOG(LogTemp, Warning, TEXT("무기 줍기 완료 → 새로운 무기 장착됨"));
	}
}


ACGunBase* APlayerCharacter::FindNearbyDroppedWeapon()
{
	TArray<AActor*> OverlappingActors;
	float PickupRadius = 200.0f; 

	UKismetSystemLibrary::SphereOverlapActors(
		this, GetActorLocation(), PickupRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
		ACGunBase::StaticClass(), {}, OverlappingActors
	);

	for (AActor* Actor : OverlappingActors)
	{
		ACGunBase* FoundWeapon = Cast<ACGunBase>(Actor);

		UE_LOG(LogTemp, Warning, TEXT("감지된 무기: %s | 타입: %d"), *FoundWeapon->GetName(), (int32)FoundWeapon->WeaponType);

		if (FoundWeapon &&
			((Inventory[0] && Inventory[0]->WeaponType == FoundWeapon->WeaponType) ||
				(Inventory[1] && Inventory[1]->WeaponType == FoundWeapon->WeaponType)))
		{
			UE_LOG(LogTemp, Warning, TEXT("이미 소지한 무기 카테고리와 동일: %d → 줍기 불가"), (int32)FoundWeapon->WeaponType);
			continue;
		}

		return FoundWeapon;
	}

	UE_LOG(LogTemp, Warning, TEXT("주울 수 있는 무기가 없음"));
	return nullptr;
}


void APlayerCharacter::SwitchWeaponSlot(int32 Slot)
{
	if (Slot == 0)
	{
		if (!Inventory[0])
		{
			UE_LOG(LogTemp, Error, TEXT("1번 무기가 없습니다. Inventory[0]이 nullptr 상태"));
			return;
		}

		if (CurrentWeapon == Inventory[0])
		{
			UE_LOG(LogTemp, Warning, TEXT("현재 이미 1번 무기를 들고 있음"));
			return;
		}

		HideCurrentWeapon();
		Inventory[0]->SetActorHiddenInGame(false);
		Inventory[0]->GetWeaponMesh()->SetVisibility(true);

		CurrentWeapon = Inventory[0];
		CurrentWeaponSlot = 0;

		AttachWeaponToHand(CurrentWeapon, 0);

		UE_LOG(LogTemp, Warning, TEXT("무기 변경 완료: 1번 무기로 교체 (%s)"), *CurrentWeapon->GetName());
	}
	else if (Slot == 1)
	{
		if (!Inventory[1])
		{
			UE_LOG(LogTemp, Warning, TEXT("2번 무기가 없습니다."));
			return;
		}

		if (CurrentWeapon == Inventory[1])
		{
			UE_LOG(LogTemp, Warning, TEXT("현재 이미 2번 무기를 들고 있음"));
			return;
		}

		HideCurrentWeapon();
		Inventory[1]->SetActorHiddenInGame(false);
		Inventory[1]->GetWeaponMesh()->SetVisibility(true);

		CurrentWeapon = Inventory[1];
		CurrentWeaponSlot = 1;

		AttachWeaponToHand(CurrentWeapon, 1);

		UE_LOG(LogTemp, Warning, TEXT("무기 변경 완료: 2번 무기로 교체 (%s)"), *CurrentWeapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("잘못된 슬롯 번호: %d"), Slot);
	}
}

void APlayerCharacter::SetPlayerStatus(float NewHealth, float NewShield)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	CurrentShield = FMath::Clamp(NewShield, 0.0f, MaxShield);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnShieldChanged.Broadcast(CurrentShield, MaxShield);
}

void APlayerCharacter::DisableFirstPersonShadows()
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponMesh()) // 1인칭 무기 그림자 제거
	{
		CurrentWeapon->GetWeaponMesh()->bCastDynamicShadow = false;
		CurrentWeapon->GetWeaponMesh()->CastShadow = false;
	}
}

void APlayerCharacter::HideCurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	if (CurrentWeapon && CurrentWeapon->GetWeaponMesh())
	{
		CurrentWeapon->GetWeaponMesh()->SetVisibility(false);
		CurrentWeapon->SetIsDrop(false);
	}
}

void APlayerCharacter::ResetPickupWeapon()
{
	bCanPickupWeapon = true;
	UE_LOG(LogTemp, Warning, TEXT("무기 줍기 가능"));
}

void APlayerCharacter::ResetDropWeaponCooldown()
{
	bCanDropWeapon = true;
	UE_LOG(LogTemp, Warning, TEXT("DropCurrentWeapon 쿨다운 종료, 다시 무기 드랍 가능"));
}

void APlayerCharacter::SetMouseSensitivity(float NewSensitivity)
{
	MouseSensitivity = FMath::Clamp(NewSensitivity, 0.1f, 5.0f);
	UE_LOG(LogTemp, Warning, TEXT("마우스 감도 변경: %f"), MouseSensitivity);
}

float APlayerCharacter::GetMouseSensitivity() const
{
	return MouseSensitivity;
}


void APlayerCharacter::ReloadWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
		PlayReloadAnimation();
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	JumpCount = 0;
}

void APlayerCharacter::Jump()
{
	if (CanJump())
	{
		Super::Jump();

		if (JumpSound)
		{
			UGameplayStatics::SpawnSound2D(this, JumpSound);
		}
	}
}

void APlayerCharacter::SwitchToPrimaryWeapon()
{
	SwitchWeaponSlot(0);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	SwitchWeaponSlot(1);
}

void APlayerCharacter::ApplyCardEffect(const FCardEffect& SelectedCard)
{
	//if (!SelectedCard) return;

	FCardEffect Effect = SelectedCard;//->CardEffect;
	float AppliedValue = Effect.EffectValues;

	switch (Effect.EffectType)
	{
	case ECardEffectType::DashCoolTimeDecrease:
		DashCoolDown *= (1.0f - AppliedValue * 0.01f);
		break;

	case ECardEffectType::MoveSpeedIncrease:
		NormalSpeed *= (1.0f + AppliedValue * 0.01f);
		break;

	case ECardEffectType::ShieldAmountIncrease:
		MaxShield += AppliedValue;
		break;

	case ECardEffectType::ShieldRateIncrease:
		ShieldRegenRate += AppliedValue;
		break;

	case ECardEffectType::MaxHealthIncrease:
		MaxHealth += AppliedValue;
		break;

	case ECardEffectType::AttackPowerIncrease:
	case ECardEffectType::AttackSpeedIncrease:
		ApplyEffectToGun(Effect);
		AppliedCardEffects.Add(Effect);
		break;
	case ECardEffectType::ShieldRegeneTimeDecrease:
		//미구현
		break;
	}
}

void APlayerCharacter::ApplyEffectToGun(FCardEffect Effect)
{
	ACGunBase* EquippedGun = GetEquippedGun();

	if (!EquippedGun) return;

	float AppliedValue = Effect.EffectValues;

	if (Effect.EffectType == ECardEffectType::AttackPowerIncrease)
	{
		EquippedGun->SetGunDamage(GetGunDamage() + AppliedValue);
	}
	else if (Effect.EffectType == ECardEffectType::AttackSpeedIncrease)
	{
		EquippedGun->SetGunDelay(GetGunDelay() * (1.0f - (AppliedValue * 0.01f)));
	}
}

ACGunBase* APlayerCharacter::GetEquippedGun()
{
	return CurrentWeapon;
}

void APlayerCharacter::HideFirstPersonMeshAndWeapon()
{
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false, true);
	}

	if (CurrentWeapon && CurrentWeapon->GetWeaponMesh())
	{
		CurrentWeapon->GetWeaponMesh()->SetVisibility(false, true);
	}
}

void APlayerCharacter::ShowFirstPersonMeshAndWeapon()
{
	if (GetMesh())
	{
		GetMesh()->SetVisibility(true, true);
	}

	if (CurrentWeapon && CurrentWeapon->GetWeaponMesh())
	{
		CurrentWeapon->GetWeaponMesh()->SetVisibility(true, true);
	}
}

void APlayerCharacter::HandlePlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("HandlePlayerDeath() 진입"));

	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandlePlayerDeath() 실행 중단 - 이미 사망한 상태 (bIsDead = true)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HandlePlayerDeath() 실행됨 - 플레이어 사망 처리 시작"));

	bIsDead = true;

	//1인칭 메시와 총기 숨기기
	HideFirstPersonMeshAndWeapon();

	//3인칭 메시 Ragdoll 활성화
	if (ThirdPersonMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("3인칭 메시 Ragdoll 적용 시작"));

		ThirdPersonMesh->SetCollisionProfileName(TEXT("Ragdoll"));
		ThirdPersonMesh->SetSimulatePhysics(true);
		ThirdPersonMesh->SetAllBodiesSimulatePhysics(true);
		ThirdPersonMesh->SetAllBodiesPhysicsBlendWeight(1.0f);
		ThirdPersonMesh->SetEnableGravity(true);
	}

	SwitchToDeathCamera();

	EnableMouseControl();
}

void APlayerCharacter::SwitchToDeathCamera()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && DeathCamera)
	{
		// 기존 1인칭 카메라 비활성화
		if (CameraComp) // 기존 1인칭 카메라가 존재하면
		{
			CameraComp->SetActive(false); // 1인칭 카메라 비활성화
			UE_LOG(LogTemp, Warning, TEXT("1인칭 카메라 비활성화됨"));
		}

		// 3인칭 카메라 활성화
		DeathCamera->SetActive(true);

		// 카메라 전환 실행
		PlayerController->SetViewTargetWithBlend(this, 0.5f, VTBlend_Cubic);

		UE_LOG(LogTemp, Warning, TEXT("사망 시 3인칭 카메라로 전환됨"));
	}
}

void APlayerCharacter::EnableMouseControl()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);

		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeUIOnly());

		UE_LOG(LogTemp, Warning, TEXT("마우스 UI 조작 활성화됨"));
	}
}

void APlayerCharacter::PlayReloadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
	}
}