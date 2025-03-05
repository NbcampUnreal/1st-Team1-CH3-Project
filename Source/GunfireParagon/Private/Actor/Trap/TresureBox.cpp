#include "Actor/Trap/TresureBox.h"
#include "GameMode/FPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

ATresureBox::ATresureBox()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsOpen = false;

	// 네트워크 동기화 설정
	bReplicates = true;
}

void ATresureBox::BeginPlay()
{
	Super::BeginPlay();
    
	// DropManager 가져오기
	AFPSGameMode* GameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		DropManager = GameMode->GetDropManager();
	}

	// 애님 인스턴스 가져오기
	if (USkeletalMeshComponent* MeshComp = FindComponentByClass<USkeletalMeshComponent>())
	{
		AnimInstance = MeshComp->GetAnimInstance();
	}
}

// **상자를 여는 함수**
void ATresureBox::OpenBox()
{
	if (bIsOpen) return;  // 이미 열린 경우 실행 X

	bIsOpen = true;
	OnRep_IsOpen();  // 애니메이션 실행

	if (HasAuthority())
	{
		OnRep_IsOpen();
	}
}

// **네트워크 클라이언트가 상태를 감지할 함수**
void ATresureBox::OnRep_IsOpen()
{
	if (bIsOpen && AnimInstance && OpenMontage)
	{
		AnimInstance->Montage_Play(OpenMontage);
	}
}

// **네트워크 동기화 설정**


// **애니메이션이 끝난 후 아이템 드롭**
void ATresureBox::OnAnimationFinished()
{
	if (DropManager)
	{
		FVector Location = GetActorLocation();
		DropManager->RandomItemDrop(Location);
	}
}