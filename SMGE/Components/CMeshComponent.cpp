#include "CMeshComponent.h"

namespace SMGE
{
	CMeshComponent::CMeshComponent(CObject* outer) : CDrawComponent(outer)
	{
	}

	CMeshComponent::CMeshComponent(CObject* outer, const CWString& modelAssetPath) : CDrawComponent(outer, modelAssetPath)
	{
	}
};
