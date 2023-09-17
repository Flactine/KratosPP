﻿#pragma once

#include <Common/Components/Component.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

template <typename TExt, typename TStatus, typename TBase>
static bool TryGetStatus(TBase* p, TStatus*& status)
{
	auto* ext = TExt::ExtMap.Find(p);
	if (ext)
	{
		status = ext->_GameObject->GetComponent<TStatus>();
		return status != nullptr;
	}
	return false;
}

template <typename TExt, typename TStatus, typename TBase>
static TStatus* GetStatus(TBase* p)
{
	TStatus* status = nullptr;
	TryGetStatus<TExt>(p, status);
	return status;
}

template <typename TypeExt, typename TypeData, typename TBase>
static bool TryGetTypeData(TBase* p, TypeData*& data)
{
	auto* ext = TypeExt::ExtMap.Find(p);
	if (ext)
	{
		INIConfigReader<TypeData>* pTypeData = static_cast<INIConfigReader<TypeData>*>(ext->pTypeData);
		if (!pTypeData)
		{
			pTypeData = INI::GetConfig<TypeData>(INI::Rules, p->ID);
			ext->pTypeData = pTypeData;
		}
		data = pTypeData->Data;
		return data != nullptr;
	}
	return false;
}

template <typename TypeExt, typename TypeData, typename TBase>
static TypeData* GetTypeData(TBase* p)
{
	TypeData* data = nullptr;
	TryGetTypeData<TypeExt>(p, data);
	return data;
}