﻿#pragma once

#include <string>
#include <format>
#include <codecvt>

#include <Extension.h>
#include <TechnoClass.h>

#include <Utilities/Debug.h>

#include <Common/Components/ScriptComponent.h>
#include <Common/EventSystems/EventSystem.h>
#include <Extension/BulletExt.h>
#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include "BulletType/Trajectory.h"

enum class BulletType
{
	UNKNOWN = 0,
	INVISO = 1,
	ARCING = 2,
	MISSILE = 3,
	ROCKET = 4,
	NOROT = 5,
	BOMB = 6
};

/// @brief 抛射体的生存属性
struct BulletLife
{
public:
	bool Interceptable = false; // 可被伤害
	int Strength = -1;			// 自定义血量

	int Health = 1;			 // 生命值
	bool IsDetonate = false; // 已损毁
	bool IsHarmless = false; // 无害
	bool SkipAE = false;	 // 爆炸不赋予AE

	void Read(INIBufferReader *ini)
	{
		this->Interceptable = ini->Get("Interceptable", Interceptable);
		this->Strength = ini->Get("Strength", Strength);
		if (Strength > 0)
		{
			this->Health = Strength;
		}
	}

	/// @brief 直接摧毁
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void Detonate(bool harmless = false, bool skipAE = false)
	{
		this->Health = -1;
		this->IsDetonate = true;
		this->IsHarmless = harmless;
		this->SkipAE = skipAE;
	}

	/// @brief 收到伤害
	/// @param damage 伤害数值
	/// @param harmless 无害
	/// @param skipAE 不赋予AE
	void TakeDamage(int damage, bool harmless, bool skipAE = false)
	{
		this->Health -= damage;
		this->IsDetonate = this->Health <= 0;
		this->IsHarmless = harmless;
		if (IsDetonate)
		{
			this->SkipAE = skipAE;
		}
	}
};

/// @brief 对抛射体的伤害属性
struct BulletDamage
{
public:
	int Damage = 0;		   // 伤害
	bool Eliminate = true; // 一击必杀
	bool Harmless = false; // 和平处置
};

/// @brief base compoment, save the Techno status
class BulletStatus : public BulletScript
{
public:
	BulletStatus(Extension<BulletClass> *ext) : BulletScript(ext)
	{
		this->Name = typeid(this).name();
	}

	TrajectoryData *GetTrajectoryData();

	BulletType GetBulletType();

	bool IsArcing();
	bool IsMissile();
	bool IsRocket();
	bool IsBomb();

	virtual void Awake() override;

	void TakeDamage(int damage, bool eliminate, bool harmless, bool checkInterceptable = false);

	void TakeDamage(BulletDamage damageData, bool checkInterceptable = false);

	void ResetTarget(AbstractClass *pNewTarget, CoordStruct targetPos);

	void ResetArcingVelocity(float speedMultiple = 1.0f, bool force = false);

	virtual void OnPut(CoordStruct *pLocation, DirType dir) override;

	void InitState_Trajectory_Missile();
	void InitState_Trajectory_Straight();

	void InitState_BlackHole();
	void InitState_Bounce();
	void InitState_DestroySelf();
	void InitState_ECM();
	void InitState_GiftBox();
	void InitState_Paintball();
	void InitState_Proximity();

	virtual void OnUpdate() override;

	void OnUpdate_Trajectory_Arcing();
	void OnUpdate_Trajectory_Bounce();
	void OnUpdate_Trajectory_Straight();
	void OnUpdate_Trajectory_Decroy();

	void OnUpdate_DestroySelf();

	void OnUpdate_BlackHole();
	void OnUpdate_ECM();
	void OnUpdate_GiftBox();
	void OnUpdate_RecalculateStatus();
	void OnUpdate_SelfLaunchOrPumpAction();

	virtual void OnUpdateEnd() override;

	void OnUpdateEnd_BlackHole(CoordStruct &sourcePos);
	void OnUpdateEnd_Proximity(CoordStruct &sourcePos);

	virtual void OnDetonate(CoordStruct *pCoords, bool &skip) override;

	bool OnDetonate_Bounce(CoordStruct *pCoords);
	bool OnDetonate_GiftBox(CoordStruct *pCoords);
	bool OnDetonate_SelfLaunch(CoordStruct *pCoords);

	virtual void OnUnInit() override;

	TechnoClass *pSource = nullptr;
	HouseClass *pSourceHouse = nullptr;

	BulletLife life = {};
	BulletDamage damage = {};

	bool SubjectToGround = false;

	bool IsBounceSplit = false;

	ObjectClass *pFakeTarget = nullptr;

	static std::vector<BulletClass *> TargetAircraftBullets;

	virtual void InvalidatePointer(void *ptr) override
	{
		AnnounceInvalidPointer(this->pSource, ptr);
		AnnounceInvalidPointer(this->pSourceHouse, ptr);
		AnnounceInvalidPointer(this->pFakeTarget, ptr);
	};

#pragma region Save/Load
	template <typename T>
	void Serialize(T &stream)
	{
		stream
			.Process(this->pSource)
			.Process(this->pSourceHouse)
			.Process(this->life)
			.Process(this->damage)
			.Process(this->SubjectToGround)
			.Process(this->IsBounceSplit)
			.Process(this->pFakeTarget)
			.Process(this->TargetAircraftBullets)
			.Process(this->_initFlag)
			;
	};

	virtual void LoadFromStream(ExStreamReader &stream) override
	{
		Component::LoadFromStream(stream);
		this->Serialize(stream);
	}

	virtual void SaveToStream(ExStreamWriter &stream) override
	{
		Component::SaveToStream(stream);
		this->Serialize(stream);
	}
#pragma endregion
private:
	BulletType _bulletType = BulletType::UNKNOWN;
	// 弹道配置
	TrajectoryData *_trajectoryData = nullptr;

	bool _initFlag = false;
};

// Helper
static bool IsDead(BulletClass *pBullet)
{
	BulletStatus *status = nullptr;
	return !pBullet || !pBullet->Type || pBullet->Health <= 0 || !pBullet->IsAlive || !TryGetStatus<BulletExt>(pBullet, status) || status->life.IsDetonate;
}

static bool IsDeadOrInvisible(BulletClass *pBullet)
{
	return IsDead(pBullet) || pBullet->InLimbo;
}

/// @brief 获取抛射体的轨迹类型：
/// Inviso优先级最高；
/// Arcing 和 ROT>0 一起写，无法发射；
/// Arcing 和 ROT=0 一起写，是抛物线；
/// Arcing 和 Vertical 一起写，无法发射；
/// ROT>0 和 Vertical 一起写，是导弹；
/// ROT=0 和 Vertical 一起写，是垂直，SHP会变直线导弹，VXL会垂直下掉。
static BulletType WhatTypeAmI(BulletClass *pBullet)
{
	BulletTypeClass *pType = nullptr;
	if (pBullet && (pType = pBullet->Type))
	{
		if (pType->Inviso)
		{
			// Inviso 优先级最高
			return BulletType::INVISO;
		}
		else if (pType->ROT > 0)
		{
			// 导弹类型
			if (pType->ROT == 1)
			{
				return BulletType::ROCKET;
			}
			return BulletType::MISSILE;
		}
		else if (pType->Vertical)
		{
			// 炸弹
			return BulletType::BOMB;
		}
		else if (pType->Arcing)
		{
			// 最后是Arcing
			return BulletType::ARCING;
		}
		else if (pType->ROT == 0)
		{
			// 再然后还有一个ROT=0的抛物线，但不是Arcing
			return BulletType::NOROT;
		}
	}
	return BulletType::UNKNOWN;
}