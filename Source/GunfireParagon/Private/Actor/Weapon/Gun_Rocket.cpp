#include "Actor/Weapon/Gun_Rocket.h"
#include "Actor/Bullet/BulletBase.h"



AGun_Rocket::AGun_Rocket()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponType = EWeaponType::Rocket;
    AmmoType = EAmmoType::Bomb;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    if (!WeaponMesh)
    {
        WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
        WeaponMesh->SetupAttachment(RootComponent);
    }

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> RocketMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Test/TestGun/m16-a2-Rocket/source/SKM_m16a21.SKM_m16a21'"));
    if (RocketMesh.Succeeded())
    {
        WeaponMesh->SetSkeletalMesh(RocketMesh.Object);
    }
}


