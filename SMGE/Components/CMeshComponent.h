#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"

namespace SMGE
{
	class CMeshComponent : public CDrawComponent
	{
	public:
		CMeshComponent();
		CMeshComponent(const CWString& modelAssetPath);
	};
};
