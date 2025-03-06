#include "AI/ENemy/EliteMeleeEnemy.h"
#include "AI/AIController/BaseEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEliteMeleeEnemy::AEliteMeleeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 40.0f;
    AttackRange = 300.0f;
    AttackDelay = 0.5f;
    MaxHealth = 400.0f;
    BaseWalkSpeed = 700.0f;
    CurrentHealth = MaxHealth;
    EXP = 20.0f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    EnemyName = "EliteMelee";
}
