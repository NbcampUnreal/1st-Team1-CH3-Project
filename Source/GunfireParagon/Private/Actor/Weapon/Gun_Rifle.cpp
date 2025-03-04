#include "Actor/Weapon/Gun_Rifle.h"
#include "Actor/Bullet/BulletBase.h"



AGun_Rifle::AGun_Rifle()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponType = EWeaponType::Rifle;

    AmmoType =EAmmoType::Normal;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    if (!WeaponMesh)
    {
        WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
        WeaponMesh->SetupAttachment(RootComponent);
    }

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Test/TestGun/m16-a2-rifle/source/SKM_m16a21.SKM_m16a21'"));
    if (RifleMesh.Succeeded())
    {
        WeaponMesh->SetSkeletalMesh(RifleMesh.Object);
    }
}
