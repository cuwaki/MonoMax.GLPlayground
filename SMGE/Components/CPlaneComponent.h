#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	// 평면는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

	class CPlaneComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CPlaneComponent)

	public:
		using This = CPlaneComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CPlaneComponent(CObject* outer);

		virtual void ReadyToDrawing() override;

		glm::vec3 getNormal(bool isWorld) const;
		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) override;
		
		void SetBoundLocalSpace(const glm::vec3& ccw_p0, const glm::vec3& ccw_p1, const glm::vec3& ccw_p2);
		void Ctor();

	protected:
		SPlaneBound PlaneBound_;
	};
};
