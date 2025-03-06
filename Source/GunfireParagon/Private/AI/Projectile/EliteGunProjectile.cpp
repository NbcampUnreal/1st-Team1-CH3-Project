#include "AI/Projectile/EliteGunProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEliteGunProjectile::AEliteGunProjectile()
{
	DamageMultiplier = 1.0f;
	Speed = 2200.0f;

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = 0.1f;
}
