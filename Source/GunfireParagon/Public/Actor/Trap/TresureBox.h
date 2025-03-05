#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TresureBox.generated.h"

UCLASS()
class GUNFIREPARAGON_API ATresureBox : public AActor
{
	GENERATED_BODY()

public:
	ATresureBox();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OpenBox();

	UFUNCTION(BlueprintCallable)
	void OnAnimationFinished();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Box State")
	bool bIsOpen;  // 상자가 열렸는지 여부

	UFUNCTION()
	void OnRep_IsOpen();  // 상태가 변경되었을 때 호출될 함수

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* OpenMontage;  // 상자 여는 애니메이션

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimInstance* AnimInstance;

	UPROPERTY()
	class ADropManager* DropManager;
};