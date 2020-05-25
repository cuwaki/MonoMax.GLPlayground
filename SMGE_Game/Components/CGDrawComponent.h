#pragma once

#include "../GCommonIncludes.h"
#include "CGComponent.h"
#include "../Assets/CGAsset.h"
#include "../Assets/CGModelData.h"

namespace SMGE
{
	class CGDrawComponent : public CGComponent
	{
	public:
		CGDrawComponent();
		CGDrawComponent(const CWString& modelAssetPath);

		virtual void ReadyToDrawing(const CWString& modelAssetPath);
		
		virtual void Tick(float td);
		virtual void Render(float td);

		glm::mat4& getTransform();
		glm::vec3& getLocation();
		glm::vec3& getDirection();
		glm::vec3& getScale();

	protected:
		// reflection
		CWString drawingModelAssetPath_;
		
		glm::mat4 objectTransform_{ 1 };
		glm::vec3 objectLocation_{ 0 };
		glm::vec3 objectDirection_{ 1, 0, 0 };
		glm::vec3 objectScale_{ 1 };

	protected:
		// runtime
		CSharPtr<CGAsset<CGModelData>> drawingModelAsset_;
	};
};
