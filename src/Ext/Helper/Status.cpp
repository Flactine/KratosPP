﻿#include "Status.h"
#include <Ext/Helper/CastEx.h>

#include <Ext/AnimStatus.h>
#include <Ext/BulletStatus.h>
#include <Ext/TechnoStatus.h>

#pragma endregion AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse)
{
	pAnim->Owner = pHouse;
	return pAnim;
}

AnimClass *SetAnimOwner(AnimClass *pAnim, TechnoClass *pTechno)
{
	pAnim->Owner = pTechno->Owner;
	return pAnim;
}

AnimClass *SetAnimOwner(AnimClass *pAnim, BulletClass *pBullet)
{
	if (BulletStatus *status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		pAnim->Owner = status->pSourceHouse;
	}
	return pAnim;
}

AnimClass *SetAnimCreater(AnimClass *pAnim, TechnoClass *pTechno)
{
	if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
	{
		status->pCreater = pTechno;
	}
	return pAnim;
}

AnimClass *SetAnimCreater(AnimClass *pAnim, BulletClass *pBullet)
{
	TechnoClass *Source = pBullet->Owner;
	if (!IsDead(Source))
	{
		if (AnimStatus *status = GetStatus<AnimExt, AnimStatus>(pAnim))
		{
			status->pCreater = Source;
		}
	}
	return pAnim;
}
#pragma endregion

#pragma endregion TechnoClass
bool IsDead(TechnoClass* pTechno)
{
	return !pTechno || !pTechno->GetType() || pTechno->Health <= 0 || !pTechno->IsAlive || pTechno->IsCrashing || pTechno->IsSinking;
}

bool IsDeadOrInvisible(TechnoClass* pTechno)
{
	return IsDead(pTechno) || pTechno->InLimbo;
}
#pragma endregion

#pragma endregion BulletClass
bool IsDead(BulletClass* pBullet)
{
	BulletStatus* status = nullptr;
	return !pBullet || !pBullet->Type || pBullet->Health <= 0 || !pBullet->IsAlive || !TryGetStatus<BulletExt>(pBullet, status) || !status || status->life.IsDetonate;
}

bool IsDeadOrInvisible(BulletClass* pBullet)
{
	return IsDead(pBullet) || pBullet->InLimbo;
}

DirStruct Facing(BulletClass* pBullet, CoordStruct location)
{
	CoordStruct source = location;
	if (location == CoordStruct::Empty)
	{
		source = pBullet->GetCoords();
	}
	CoordStruct forward = source + ToCoordStruct(pBullet->Velocity);
	return Point2Dir(source, forward);
}

#pragma endregion
