#pragma once

#include "../GCommonIncludes.h"
#include "CGDrawComponent.h"
#include "../Assets/CGStaticMeshAsset.h"

namespace MonoMaxGraphics
{
	class CGStaticMeshComponent : public CGDrawComponent
	{
	public:
		CGStaticMeshComponent();

		//CSharPtr<CGStaticMeshAsset> asset_;
	};
};
