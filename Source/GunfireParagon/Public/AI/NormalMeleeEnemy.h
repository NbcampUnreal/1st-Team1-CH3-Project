#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "NormalMeleeEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ANormalMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	ANormalMeleeEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void Attack(const FVector& TargetLocation) override;
	virtual void PerformMeleeAttack(const FVector& TargetLocation) override;

protected:
	virtual void BeginPlay() override;
	virtual void PerformRangeAttack(const FVector& TargetLocation) override;

	FVector LastKnownPlayerLocation;
};
