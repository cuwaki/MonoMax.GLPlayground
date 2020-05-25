#pragma once

#include "../GCommonIncludes.h"
#include "CGDrawComponent.h"

namespace SMGE
{
	class CGMeshComponent : public CGDrawComponent
	{
	public:
		CGMeshComponent();
		CGMeshComponent(const CWString& modelAssetPath);
	};
};
