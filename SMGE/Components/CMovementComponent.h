#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"
#include "../CInterpolation.h"

namespace SMGE
{
	class CMovementComponent : public CComponent
	{
		DECLARE_RTTI_CObject(CMovementComponent)

	public:
		using This = CMovementComponent;
		using Super = CComponent;
		using TReflectionStruct = SGRefl_Component;

		friend struct TReflectionStruct;

	public:
		CMovementComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void Tick(float td) override;

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		std::unique_ptr<TReflectionStruct> reflTransformCompo_;

		class CActor* actorParent_;
		nsRE::Transform* targetTransform_ = nullptr;

		// 테스트 코드 - 인터폴레이션
		CInterpolation<glm::vec3> interpTranslation_;
		CInterpolation<glm::vec3> interpRotation_;
		CInterpolation<glm::vec3> interpScale_;
	};
}
