#pragma once

#include "../GCommonIncludes.h"
#include "CGDrawComponent.h"

namespace MonoMaxGraphics
{
	class CGMeshComponent : public CGDrawComponent
	{
	public:
		CGMeshComponent();
		CGMeshComponent(const CWString& modelAssetPath);
	};
};
