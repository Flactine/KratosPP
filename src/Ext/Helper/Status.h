﻿#pragma once

#include <GeneralStructures.h>
#include <AnimClass.h>
#include <BulletClass.h>
#include <HouseClass.h>
#include <TechnoClass.h>

#pragma region AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse);
AnimClass* SetAnimOwner(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimOwner(AnimClass* pAnim, BulletClass* pBullet);

AnimClass* SetAnimCreater(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimCreater(AnimClass* pAnim, BulletClass* pBullet);
#pragma endregion

#pragma region TechnoClass
bool IsDead(TechnoClass* pTechno);
bool IsDeadOrInvisible(TechnoClass* pTechno);
#pragma endregion

#pragma region BulletClass
bool IsDead(BulletClass* pBullet);
bool IsDeadOrInvisible(BulletClass* pBullet);

DirStruct Facing(BulletClass* pBullet, CoordStruct location = CoordStruct::Empty);
#pragma endregion
