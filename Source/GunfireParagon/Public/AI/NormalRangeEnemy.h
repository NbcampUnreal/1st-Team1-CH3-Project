#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "NormalRangeEnemy.generated.h"

UCLASS()
class GUNFIREPARAGON_API ANormalRangeEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ANormalRangeEnemy();

	virtual void Attack() override;

protected:
	virtual void PerformMeleeAttack() override;
	virtual void PerformRangeAttack() override;
};
