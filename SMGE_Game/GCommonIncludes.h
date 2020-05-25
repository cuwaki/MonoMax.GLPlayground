#pragma once

#include "../SMGE/GECommonIncludes.h"

#include "Components/CGComponent.h"
#include "Interfaces/CGInterface.h"

namespace SMGE
{
	using TActorKey = uint32;

	const TActorKey InvalidActorKey = 0;

	inline bool IsValidActorKey(const TActorKey& ak)
	{
		return ak > 0;
	}
};
