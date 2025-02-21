// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/Gun_Rifle.h"
#include "Actor/Bullet/BulletBase.h"

AGun_Rifle::AGun_Rifle()
{
    PrimaryActorTick.bCanEverTick = false;

	AmmoType = EAmmoType::Normal;
}

void AGun_Rifle::BeginPlay()
{
	Super::BeginPlay();
}

void AGun_Rifle::Fire()
{
	Super::Fire();

	
}
