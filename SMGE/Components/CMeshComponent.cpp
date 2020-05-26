#include "CMeshComponent.h"

namespace SMGE
{
	CMeshComponent::CMeshComponent() : CDrawComponent()
	{
	}

	CMeshComponent::CMeshComponent(const CWString& modelAssetPath) : CDrawComponent(modelAssetPath)
	{
	}
};
