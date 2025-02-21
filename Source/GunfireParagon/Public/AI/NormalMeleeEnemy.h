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
	virtual void Attack() override;

protected:
	virtual void PerformMeleeAttack() override;
	virtual void PerformRangeAttack() override;

};
