#pragma once

#include "../SMGE/GECommonIncludes.h"

#include "Components/CGComponent.h"
#include "Interfaces/CGInterface.h"

namespace MonoMaxGraphics
{
	using ActorKey = uint32;

	const ActorKey InvalidActorKey = 0;

	inline bool IsValidActorKey(const ActorKey& ak)
	{
		return ak > 0;
	}
};
