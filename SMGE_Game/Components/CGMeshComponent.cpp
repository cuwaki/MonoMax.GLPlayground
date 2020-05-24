#include "CGMeshComponent.h"

namespace MonoMaxGraphics
{
	CGMeshComponent::CGMeshComponent() : CGDrawComponent()
	{
	}

	CGMeshComponent::CGMeshComponent(const CWString& modelAssetPath) : CGDrawComponent(modelAssetPath)
	{
	}
};
