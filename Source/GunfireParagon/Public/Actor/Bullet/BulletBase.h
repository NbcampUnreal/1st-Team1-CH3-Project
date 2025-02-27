#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"  
#include "NiagaraFunctionLibrary.h"  
#include "Components/DecalComponent.h"
#include "BulletBase.generated.h"


class ABulletPool;


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Pierce UMETA(DisplayName = "Pierce"),
	Bomb UMETA(DisplayName = "Bomb")
};

UCLASS()
class GUNFIREPARAGON_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	FVector NormalImpulse, const FHitResult& Hit);

	void SpawnBulletDecal(const FHitResult& Hit);

public:
	virtual void Fire(FVector StartLocation, FVector Direction, float GunDamage);

	float GetSpawnTime() const { return SpawnTime; } 
	void SetSpawnTime(float Time) { SpawnTime = Time; }
	
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "BulletMesh")
	UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Bullet Status")
	float BulletDamage = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	UMaterial* BulletDecalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	FVector DecalSize= {5.0f,5.0f,5.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	float DecalLifeTime = 5.0f;
	// Test pool
	UPROPERTY()
	ABulletPool* BulletPool;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement; //  총알 이동 컴포넌트 추가
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Properties")
	EAmmoType AmmoType; //  총알 타입을 저장

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Projectile")
	USphereComponent* CollisionComponent;
	
	float SpawnTime; 
	
};
