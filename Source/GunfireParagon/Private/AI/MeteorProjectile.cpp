#include "AI/MeteorProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMeteorProjectile::AMeteorProjectile()
{
	DamageMultiplier = 3.0f;
	Speed = 2000.0f;

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;

}
