#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../Assets/CAsset.h"
#include "../Assets/CModelData.h"

namespace SMGE
{
	class CDrawComponent : public CComponent
	{
	public:
		CDrawComponent(CObject* outer);
		CDrawComponent(CObject* outer, const CWString& modelAssetPath);

		virtual void Tick(float td);
		virtual void Render(float td);

		virtual void OnBeginPlay(class CActor* parent) override;
		virtual void OnEndPlay() override;

		virtual void ReadyToDrawing();
		void SetDrawingModelAsset(const CWString& modelAssetPath);

		glm::mat4& getTransform();
		glm::vec3& getLocation();
		glm::vec3& getDirection();
		glm::vec3& getScale();

	protected:
		class nsRE::CRenderingEngine* GetRenderingEngine();

	protected:
		// reflection
		CWString drawingModelAssetPath_;
		
		glm::mat4 objectTransform_{ 1 };
		glm::vec3 objectLocation_{ 0 };
		glm::vec3 objectDirection_{ 1, 0, 0 };
		glm::vec3 objectScale_{ 1 };

	protected:
		// runtime
		CSharPtr<CAsset<CModelData>> drawingModelAsset_;

		// 여기 수정
		//nsRE::OldModelWorld* myWorldModel_ = nullptr;
	};
};
