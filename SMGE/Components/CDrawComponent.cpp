#include "CDrawComponent.h"
#include "../Assets/CAssetManager.h"

namespace SMGE
{
	CDrawComponent::CDrawComponent() : CComponent()
	{
	}

	CDrawComponent::CDrawComponent(const CWString& modelAssetPath) : CComponent()
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CDrawComponent::ReadyToDrawing(const CWString& modelAssetPath)
	{
		drawingModelAssetPath_ = modelAssetPath;
		drawingModelAsset_ = CAssetManager::LoadAsset<CModelData>(drawingModelAssetPath_);
	}

	void CDrawComponent::Tick(float td)
	{
	}

	void CDrawComponent::Render(float td)
	{
	}

	glm::mat4& CDrawComponent::getTransform()
	{
		return objectTransform_;
	}
	glm::vec3& CDrawComponent::getLocation()
	{
		return objectLocation_;
	}
	glm::vec3& CDrawComponent::getDirection()
	{
		return objectDirection_;
	}
	glm::vec3& CDrawComponent::getScale()
	{
		return objectScale_;
	}
};
