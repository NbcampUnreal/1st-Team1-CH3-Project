// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MonsterWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNFIREPARAGON_API UMonsterWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UMonsterWidgetComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UIngameTargetHUD> TargetHUDClass;
	
	class UIngameTargetHUD* TargetHUD;

	UFUNCTION()
	void UpdateScale(float Distance);

	UFUNCTION()
	void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION()
	void UpdateName(FString TargetName);

	UFUNCTION()
	void UpdateForwardVector();

	UFUNCTION()
	ACharacter* GetSinglePlayer();
	
	UFUNCTION()
	float GetPlayerToDistance();

	UFUNCTION()
	void ConvertVisibilityMode();
	
	UFUNCTION()
	void ConvertTickRate();

	FTimerHandle VisibleTimer;;

private:
	bool IsSHowHUD;
};
