// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/Gun_Sniper.h"
#include "Actor/Bullet/BulletBase.h"
AGun_Sniper::AGun_Sniper()
{
	bIsAutoFire = false;
	
	AmmoType = EAmmoType::Pierce;
}

