#pragma once

#include "TypeExtension.h"

#include <TechnoTypeClass.h>

class TechnoTypeExt : public TypeExtension<TechnoTypeClass, TechnoTypeExt>
{
public:
	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0xDF4;

	static TechnoTypeExt::ExtContainer ExtMap;
};