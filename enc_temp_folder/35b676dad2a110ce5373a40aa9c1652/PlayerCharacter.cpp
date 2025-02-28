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

	Tags.Add("Player");
}

//ReturnHPValue();
//ReturnShieldValue();

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;

	NormalSpeed = 1000.0f;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;


	if (DefaultWeaponClass)
	{
		ACGunBase* DefaultWeapon = GetWorld()->SpawnActor<ACGunBase>(DefaultWeaponClass);

		if (DefaultWeapon)
		{
			Inventory[0] = DefaultWeapon;
			CurrentWeapon = Inventory[0];

			CurrentWeaponSlot = 0;

			AttachWeaponToHand(CurrentWeapon, 0);

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

	GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &APlayerCharacter::ResetDash, DashCooldown, false);

	UE_LOG(LogTemp, Warning, TEXT("대시 실행: 방향 = %s, 속도 = %f"), *DashDirection.ToString(), DashSpeed);
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

	if (GetWorldTimerManager().IsTimerActive(DashCooldownTimer))
	{
		GetWorldTimerManager().ClearTimer(DashCooldownTimer);
	}

	UE_LOG(LogTemp, Warning, TEXT("대시 쿨다운 종료, 다시 대시 가능"));
}

void APlayerCharacter::IncreaseMouseSensitivity()
{
	SetMouseSensitivity(MouseSensitivity + 0.1f);
}

void APlayerCharacter::DecreaseMouseSensitivity()
{
	SetMouseSensitivity(MouseSensitivity - 0.1f);
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
	}
}

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
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

	//기존 무기 상태 로그 추가
	UE_LOG(LogTemp, Warning, TEXT(" EquipWeapon 실행 - 교체 대상: %s"), *NewWeapon->GetName());

	//현재 장착된 무기가 1번 슬롯(기본 무기)이라면, 기본 무기를 숨김 처리
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

	//새로운 무기를 2번 슬롯에 저장
	Inventory[1] = NewWeapon;
	CurrentWeapon = Inventory[1];

	//현재 슬롯을 2번으로 변경
	CurrentWeaponSlot = 1;

	//새로운 무기 부착
	AttachWeaponToHand(CurrentWeapon, 1);

	//무기 장착 후 상태 확인
	UE_LOG(LogTemp, Warning, TEXT("🚨 EquipWeapon 실행 후 CurrentWeapon: %s"), *CurrentWeapon->GetName());

	return true;
}






void APlayerCharacter::DropCurrentWeapon(int32 Slot)
{
	if (Slot == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("기본 무기(1번 슬롯)는 드랍할 수 없습니다."));
		return;
	}

	if (!bCanDropWeapon) //쿨타임 중이면 실행하지 않음
	{
		UE_LOG(LogTemp, Warning, TEXT("DropCurrentWeapon 실행 방지: 쿨다운 중"));
		return;
	}

	ACGunBase*& WeaponSlot = Inventory[1]; // 2번 슬롯 무기
	if (!WeaponSlot)
	{
		return;
	}

	//드랍 가능 플래그 OFF & 타이머 설정
	bCanDropWeapon = false;
	GetWorldTimerManager().SetTimer(DropWeaponCooldownTimer, this, &APlayerCharacter::ResetDropWeaponCooldown, 0.5f, false);

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	FRotator DropRotation = GetActorRotation();

	if (WeaponSlot->GetWeaponMesh())
	{
		WeaponSlot->GetWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		//물리 적용 활성화
		WeaponSlot->GetWeaponMesh()->SetSimulatePhysics(true);
		WeaponSlot->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	WeaponSlot->SetActorLocation(DropLocation);
	WeaponSlot->SetActorRotation(DropRotation);
	WeaponSlot->SetActorHiddenInGame(false);
	WeaponSlot->SetActorEnableCollision(true);

	FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, 200.0f);
	WeaponSlot->GetWeaponMesh()->AddImpulse(DropImpulse, NAME_None, true);

	DroppedWeapons.Add(WeaponSlot);

	//Inventory[1]을 즉시 nullptr로 변경
	Inventory[1] = nullptr;
}


void APlayerCharacter::AttachWeaponToHand(ACGunBase* NewWeapon, int32 Slot)
{
	if (!NewWeapon || !NewWeapon->GetWeaponMesh())
	{
		UE_LOG(LogTemp, Error, TEXT("AttachWeaponToHand 실패: WeaponMesh가 NULL임!"));
		return;
	}

	//기존 무기 숨기기
	HideCurrentWeapon();

	//손에 부착할 소켓 선택
	FName HandSocket = TEXT("WeaponSocket");
	switch (NewWeapon->WeaponType)
	{
	case EWeaponType::Rifle: HandSocket = TEXT("RifleSocket"); break;
	case EWeaponType::Shotgun: HandSocket = TEXT("ShotgunSocket"); break;
	case EWeaponType::Sniper: HandSocket = TEXT("SniperSocket"); break;
	case EWeaponType::Rocket: HandSocket = TEXT("RocketSocket"); break;
	}

	//무기 장착 직전 상태 확인
	UE_LOG(LogTemp, Warning, TEXT("🚨 무기 장착 시도: %s"), *NewWeapon->GetName());

	//무기를 손에 부착 (물리 적용 방지)
	NewWeapon->GetWeaponMesh()->SetSimulatePhysics(false);
	NewWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NewWeapon->GetWeaponMesh()->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		HandSocket
	);

	//무기 보이기
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
		//무기 교체 전에 기존 무기를 드랍
		if (SecondaryWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("기존 2번 무기 드랍: %s"), *SecondaryWeapon->GetName());

			//기존 무기 부착 해제
			SecondaryWeapon->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			//물리 시뮬레이션 활성화 (무기 드랍)
			//이 부분을 EquipWeapon 이후로 이동!
		}

		//새로운 무기 장착
		EquipWeapon(DroppedWeapon, 2);
		UE_LOG(LogTemp, Warning, TEXT("2번 무기 교체 완료, 새로운 무기: %s"), *DroppedWeapon->GetName());

		//기존 무기의 물리 활성화는 여기서 실행 (EquipWeapon 이후)
		if (SecondaryWeapon)
		{
			SecondaryWeapon->WeaponMesh->SetSimulatePhysics(true);
			SecondaryWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SecondaryWeapon->WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);

			// Impulse 적용 (살짝 튕겨나가게)
			FVector DropImpulse = GetActorForwardVector() * 300.0f + FVector(0, 0, -800.0f);
			SecondaryWeapon->WeaponMesh->AddImpulse(DropImpulse, NAME_None, true);

			UE_LOG(LogTemp, Warning, TEXT("기존 무기에 Impulse 적용 완료: %s"), *SecondaryWeapon->GetName());
		}
	}
}



void APlayerCharacter::PickupWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("G 키 입력 감지됨: 무기 줍기 시도"));

	//무기 줍기 쿨타임 체크
	if (!bCanPickupWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("G 키 연속 입력 방지: 무기 줍기 동작이 쿨다운 중입니다!"));
		return;
	}

	//쿨타임 활성화
	bCanPickupWeapon = false;
	GetWorldTimerManager().SetTimer(PickupCooldownTimer, this, &APlayerCharacter::ResetPickupWeapon, 1.0f, false);

	//주변에 무기가 있는지 확인
	ACGunBase* NearbyWeapon = FindNearbyDroppedWeapon();

	//무기가 없을 경우 예외 방지
	if (!NearbyWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("주울 수 있는 무기가 없음 → 무기 교체 불가"));
		return;
	}

	//여기서부터는 NearbyWeapon이 NULL이 아님이 보장됨
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
	float PickupRadius = 200.0f; //무기 감지 범위 조정

	UKismetSystemLibrary::SphereOverlapActors(
		this, GetActorLocation(), PickupRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
		ACGunBase::StaticClass(), {}, OverlappingActors
	);

	for (AActor* Actor : OverlappingActors)
	{
		ACGunBase* FoundWeapon = Cast<ACGunBase>(Actor);

		//현재 손에 들고 있는 무기는 무시
		if (FoundWeapon && FoundWeapon != CurrentWeapon)
		{
			return FoundWeapon;
		}
	}

	return nullptr;
}

void APlayerCharacter::SwitchWeaponSlot(int32 Slot)
{
	if (Slot == 0) //1번 무기로 변경
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
	else if (Slot == 1) //2번 무기로 변경
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


void APlayerCharacter::HideCurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	//무기 Mesh도 숨김 처리
	if (CurrentWeapon && CurrentWeapon->GetWeaponMesh())
	{
		CurrentWeapon->GetWeaponMesh()->SetVisibility(false);
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


void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	JumpCount = 0;
}

void APlayerCharacter::SwitchToPrimaryWeapon()
{
	SwitchWeaponSlot(0);
}

void APlayerCharacter::SwitchToSecondaryWeapon()
{
	SwitchWeaponSlot(1);
}