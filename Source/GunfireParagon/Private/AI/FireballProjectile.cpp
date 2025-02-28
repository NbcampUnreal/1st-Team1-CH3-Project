#include "AI/FireballProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

AFireballProjectile::AFireballProjectile()
{
	DamageMultiplier = 1.0f;
	Speed = 4500.0f;

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}



