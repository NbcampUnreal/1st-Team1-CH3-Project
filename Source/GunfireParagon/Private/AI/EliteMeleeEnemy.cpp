#include "AI/EliteMeleeEnemy.h"
#include "AI/BaseEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEliteMeleeEnemy::AEliteMeleeEnemy()
{
    AIControllerClass = ABaseEnemyAIController::StaticClass();

    Damage = 40.0f;
    AttackRange = 300.0f;
    AttackDelay = 0.5f;
    MaxHealth = 400.0f;
    BaseWalkSpeed = 800.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}
