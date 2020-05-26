#pragma once

#include "GECommonIncludes.h"

#include "Components/CComponent.h"
#include "Interfaces/CInterfaceBase.h"

namespace SMGE
{
	using TActorKey = uint32;

	const TActorKey InvalidActorKey = 0;

	inline bool IsValidActorKey(const TActorKey& ak)
	{
		return ak > 0;
	}
};
