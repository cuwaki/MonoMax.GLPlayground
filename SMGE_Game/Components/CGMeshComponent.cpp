#include "CGMeshComponent.h"

namespace SMGE
{
	CGMeshComponent::CGMeshComponent() : CGDrawComponent()
	{
	}

	CGMeshComponent::CGMeshComponent(const CWString& modelAssetPath) : CGDrawComponent(modelAssetPath)
	{
	}
};
