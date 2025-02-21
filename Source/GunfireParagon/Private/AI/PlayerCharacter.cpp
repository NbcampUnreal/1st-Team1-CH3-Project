#include "AI/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;

	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, FString::Printf(TEXT("Player Damaged: %f, CunrrentHealth: %f"), ActualDamage, Health));

	return ActualDamage;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

