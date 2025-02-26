#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMagicProjectile.generated.h"

UCLASS()
class GUNFIREPARAGON_API ABaseMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseMagicProjectile();
	
	void SetVelocity(FVector Direction);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void CheckDestroyCondition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<class USphereComponent> SphereComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<class UParticleSystemComponent> ProjectileParticle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float DamageMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* HitImpactEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* DisappearEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	USoundBase* ImpactSound;

	FVector InitialLocation;
	FTimerHandle DestroyCheckTimer;
};
