#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"

namespace SMGE
{
	class CMeshComponent : public CDrawComponent
	{
	public:
		CMeshComponent(CObject* outer);
		CMeshComponent(CObject* outer, const CWString& modelAssetPath);
	};
};
