// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapBase.generated.h"

UCLASS(Abstract)
class GUNFIREPARAGON_API ATrapBase : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trap")
	float DamageAmount = 20.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trap")
	bool bIsActive = true; 

	
public:
	ATrapBase();

protected:
	virtual void BeginPlay() override;

	// 함정 활성화 (자식 클래스에서 구현)
	virtual void ActivateTrap();

	// 충돌 시 데미지 적용
	UFUNCTION()
	virtual void OnTrapTriggered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult);

	// 데미지 적용
	virtual void ApplyDamage(AActor* Target);

};



