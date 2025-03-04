// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameMainWidget.h"
#include "Widgets\InGame\InGameMinimap.h"
#include "Widgets\InGame\IngamePlayerStatus.h"
#include "Widgets\InGame\IngameWeaponWidget.h"
#include "Widgets\InGame\IngameCrossHairs.h"
#include "Widgets\InGame\DashEffectWidget.h"
#include "Player/PlayerCharacter.h"
#include "Widgets\MinimapTracker.h"
#include "Actor\Bullet\BulletBase.h"

void UIngameMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatus Missing"));
		return;
	}
	if (!WeaponStatusWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Missing"));
		return;
	}
	if (!MinimapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Minimap Missing"));
		return;
	}
	if (!CrossHairWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("CrossHair Missing"));
		return;
	}
	if (!DashWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("DashWidget Missing"));
		return;
	}
}

void UIngameMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* CurrentPlayer = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if (!CurrentPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Current Player in nullptr"));
			return;
		}
		if (!CurrentPlayer->OnHealthChanged.IsBound())
		{
			CurrentPlayer->OnHealthChanged.AddDynamic(this, &UIngameMainWidget::OnPlayerHealthBinding);
		}
		if (!CurrentPlayer->OnShieldChanged.IsBound())
		{
			CurrentPlayer->OnShieldChanged.AddDynamic(this, &UIngameMainWidget::OnPlayerShieldBinding);
		}
		if (!CurrentPlayer->OnAmmoChanged.IsBound())
		{
			CurrentPlayer->OnAmmoChanged.AddDynamic(this, &UIngameMainWidget::OnWeaponAmmoBinding);
		}
		if (!UMinimapTracker::OnActorLocation.IsBound())
		{
			UMinimapTracker::OnActorLocation.AddDynamic(this, &UIngameMainWidget::OnMinimapUpdated);
		}
		if (!CurrentPlayer->OnDashState.IsBound())
		{
			CurrentPlayer->OnDashState.AddDynamic(this, &UIngameMainWidget::OnPlayerIsDashBinding);
		}
		if (!CurrentPlayer->OnDashCoolDown.IsBound())
		{
			CurrentPlayer->OnDashCoolDown.AddDynamic(this, &UIngameMainWidget::OnPlayerDashCooldownBinding);
		}
		if (!CurrentPlayer->OnWeaponClass.IsBound())
		{
			CurrentPlayer->OnWeaponClass.AddDynamic(this, &UIngameMainWidget::OnWeaponTextureBinding);
		}
		if (!ABulletBase::OnHitMarker.IsBound())
		{
			ABulletBase::OnHitMarker.AddDynamic(this, &UIngameMainWidget::OnHitMarkerBinding);
		}

		OnPlayerHealthBinding(CurrentPlayer->GetCurrentHealth(), CurrentPlayer->GetMaxHealth());
		OnPlayerShieldBinding(CurrentPlayer->GetCurrentShield(), CurrentPlayer->GetMaxShield());
		OnWeaponAmmoBinding(CurrentPlayer->CurrentAmmo, CurrentPlayer->MaxAmmo);
		OnWeaponTextureBinding(CurrentPlayer->GetCurrentWeaponClass());
	}

}

void UIngameMainWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ABulletBase::OnHitMarker.RemoveDynamic(this, &UIngameMainWidget::OnHitMarkerBinding);
}

void UIngameMainWidget::OnPlayerHealthBinding(float CurrentHP, float MaxHP)
{
	// Widget관련 meta를 사용한 Bind를 하였기 때문에 유효성 검사가 필요 할까?
	PlayerStatusWidget->SetCurrentHP(CurrentHP, MaxHP);
}

void UIngameMainWidget::OnPlayerShieldBinding(float CurrentShield, float MaxShield)
{
	PlayerStatusWidget->SetCurrentShield(CurrentShield, MaxShield);
}

void UIngameMainWidget::OnPlayerIsDashBinding(bool IsDash)
{
	DashWidget->ConvertVisibility(IsDash);
}

void UIngameMainWidget::OnPlayerDashCooldownBinding(float DashCoolDown)
{
	WeaponStatusWidget->SetDashCoolDown(DashCoolDown);
}

void UIngameMainWidget::OnWeaponAmmoBinding(float CurrentAmmo, float MaxAmmo)
{
	WeaponStatusWidget->SetCurrentAmmo(CurrentAmmo, MaxAmmo);
}

void UIngameMainWidget::OnWeaponTextureBinding(ACGunBase* CurrentWeapon)
{
	WeaponStatusWidget->SetCurrentWeaponTexture(CurrentWeapon);
}

void UIngameMainWidget::OnMinimapUpdated(ACharacter* Target, float Distance)
{
	if (Target->GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
	{
		MinimapWidget->RemoveMinimapIcon(Target);
	}
	else
	{
		MinimapWidget->AddMinimapIcon(Target);
	}
}

void UIngameMainWidget::OnHitMarkerBinding(bool IsHead)
{
	CrossHairWidget->SetAnimationHitMarker(IsHead);
}


/*
* TMap을 사용하여 Enum/FString의 Key를 사용하지 않은 이유
* 1. 메모리 고려
* - Update하는 로직의 경우 위젯 내 2Param의 이벤트 바인딩을하는 함수가 2~3개입니다.
* - 해당 방식을 코드 간편화를 위한 TMap을 사용시, 각 WBP에서 TMap을 선언하게 되며, PlayerCharacter내 함수 전달시에도 TMap형식으로 전달하게됩니다.
* - 이러한 경우 TMap사용이 Player + N개의 View Widget을 생성하게 되며, 또한 Enum,FString의 메모리 할당량을 고려하여 2Param(Value,Value2)를 사용하였습니다.
*
* 2. 코드 가독성 고려
* - 현재 프로젝트는 언리얼을 사용한 첫번째 팀 프로젝트입니다.
* - 현업이 아닌, 노베이스분들과 프로젝트 제작 경험이 있으신 분들이 섞여있는 상황입니다.
* - 팀원분들을 고려하여 해당 함수가 어떤 역할을 하는지에 대해 명확하게 보여줄 수 있습니다.
*
* 3. 프로젝트 중점 [프로젝트 완성]
* - 메모리 및 코드 가독성을 고려하였지만, 낮은 응집도의 함수를 보여주는 가장 큰 이유입니다.
* - 현재 주석 작성시 프로젝트 완성 후 트러블 슈팅 및 프로파일링 단계가 아닙니다.
* - 해당 로직 외에도 다른 API 들이 존재하기 때문에 간단한 로직을 사용하였습니다.
* - 프로파일링 및 트러블슈팅 단계에서 게임 성능에 영향을 주는 주 원인일 경우, 리팩토링을 할 예정입니다.
*/