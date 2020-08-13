#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"
#include "../CCurve.h"

namespace SMGE
{
	class CTransformComponent : public CComponent
	{
	public:
		using Super = CComponent;
		using TReflectionStruct = SGRefl_Component;

		friend struct TReflectionStruct;

	public:
		CTransformComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void Tick(float td) override;

		// CInt_Reflection
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflTransformCompo_;

		class CActor* actorParent_;
		nsRE::Transform* targetTransform_ = nullptr;

		CCurve<glm::vec3> curveTranslation_;
	};
}
