#include "CGDrawComponent.h"
#include "../Assets/CGAssetManager.h"

namespace MonoMaxGraphics
{
	CGDrawComponent::CGDrawComponent() : CGComponent()
	{
	}

	CGDrawComponent::CGDrawComponent(const CWString& modelAssetPath) : CGComponent()
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CGDrawComponent::ReadyToDrawing(const CWString& modelAssetPath)
	{
		drawingModelAssetPath_ = modelAssetPath;
		drawingModelAsset_ = CGAssetManager::LoadAsset<CGModelData>(drawingModelAssetPath_);
	}

	glm::mat4& CGDrawComponent::getTransform()
	{
		return objectTransform_;
	}
	glm::vec3& CGDrawComponent::getLocation()
	{
		return objectLocation_;
	}
	glm::vec3& CGDrawComponent::getDirection()
	{
		return objectDirection_;
	}
	glm::vec3& CGDrawComponent::getScale()
	{
		return objectScale_;
	}
};
