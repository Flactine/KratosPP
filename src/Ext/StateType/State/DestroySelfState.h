﻿#pragma once

#include "../StateScript.h"
#include "DestroySelfData.h"

#include <Ext/TechnoType/DamageText.h>

class DestroySelfState : public StateScript<DestroySelfData>
{
public:
	STATE_SCRIPT(DestroySelf);

	bool AmIDead();

	void DestroyNow(bool peaceful);

	virtual void Clean() override
	{
		StateScript<DestroySelfData>::Clean();

		GoDie = false;

		_safety = false; // 为子机设置的保险
		_delay = 0;
		_delayTimer = {};
	}

	virtual void OnStart() override;

	virtual void OnUpdate() override;

	DestroySelfState& operator=(const DestroySelfState& other)
	{
		if (this != &other)
		{
			StateScript<DestroySelfData>::operator=(other);
			GoDie = other.GoDie;
			_safety = other._safety;
			_delay = other._delay;
			_delayTimer = other._delayTimer;
		}
		return *this;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->GoDie)
			.Process(this->_safety)
			.Process(this->_delay)
			.Process(this->_delayTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		StateScript<DestroySelfData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		StateScript<DestroySelfData>::Save(stream);
		return const_cast<DestroySelfState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	void ExtendLife();

	bool Timeup();

	bool GoDie = false;

	bool _safety = false; // 为子机设置的保险
	int _delay = 0;
	CDTimerClass _delayTimer{};
};
