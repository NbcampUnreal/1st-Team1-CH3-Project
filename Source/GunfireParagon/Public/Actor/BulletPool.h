
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPool.generated.h"


class ABulletBase;

class ABulletBase;

UCLASS()
class GUNFIREPARAGON_API ABulletPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletPool();


	UFUNCTION()
	ABulletBase* GetPooledBullet(EAmmoType AmmoType);

	UFUNCTION()
	void ReturnBullet(ABulletBase* Bullet,EAmmoType AmmoType);

	UFUNCTION()
	void CheckBulletLifetime();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:

	//  블루프린트 클래스 참조 (블루프린트로 만든 탄환을 사용하기 위해)
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABulletBase> NormalBulletClass;

	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABulletBase> PierceBulletClass;
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	TSubclassOf<ABulletBase> BombBulletClass;
	
	
	TMap<EAmmoType, TArray<ABulletBase*>> BulletPools; //  탄환 종류별로 풀을 관리

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Bullet Pool")
	int32 PoolSize = 100;

	
	UPROPERTY(EditAnywhere, Category = "Bullet Pool")
	float MaxBulletLifetime = 3.0f; //  모든 총알의 최대 생존 시간 (초)

	UPROPERTY()
	TSet<ABulletBase*> ActiveBullets;

	FTimerHandle BulletCheckTimer; //  일정 시간마다 모든 총알을 체크하는 타이머
};
