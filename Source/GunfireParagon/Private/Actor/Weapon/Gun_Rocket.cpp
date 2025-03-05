#include "Actor/Weapon/Gun_Rocket.h"
#include "Actor/Bullet/BulletBase.h"



AGun_Rocket::AGun_Rocket()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponType = EWeaponType::Rocket;
    AmmoType = EAmmoType::Bomb;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Test/TestGun/super-mega-death-rocket/source/SKM_side1.SKM_side1'"));
	if (RifleMesh.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(RifleMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Sound/RocketFire.RocketFire"));
	if (SoundAsset.Succeeded())
	{
        UE_LOG(LogTemp, Error, TEXT("로켓 사운드 불러오지못함"));
		FireSound = SoundAsset.Object;
	}

	
	SwitchGunSound(WeaponType);
}



void AGun_Rocket::BeginPlay()
{
    Super::BeginPlay();

    if (!WeaponMesh)
    {
        return;
    }

}



void AGun_Rocket::Fire()
{
    Super::Fire();
}
