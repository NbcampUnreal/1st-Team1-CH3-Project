// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/Gun_Sniper.h"
#include "Actor/Bullet/BulletBase.h"
AGun_Sniper::AGun_Sniper()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsAutoFire = false;
	
	AmmoType = EAmmoType::Pierce;
	WeaponType = EWeaponType::Sniper;
}

