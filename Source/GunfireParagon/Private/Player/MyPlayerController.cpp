#include "Player/MyPlayerController.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	//SprintAction(nullptr),
	DashAction(nullptr),
	FireAction(nullptr),
	SwitchToPrimaryWeaponAction(nullptr),
	SwitchToSecondaryWeaponAction(nullptr),
	PickupWeaponAction(nullptr),
	IncreaseSensitivityAction(nullptr),
	DecreaseSensitivityAction(nullptr),
	ReloadAction(nullptr)
	//DebugDamageAction(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem < UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}