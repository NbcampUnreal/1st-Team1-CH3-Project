#include "AI/Projectile/MeteorProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMeteorProjectile::AMeteorProjectile()
{
	DamageMultiplier = 3.0f;
	Speed = 2500.0f;

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;

}
