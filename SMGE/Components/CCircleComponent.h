#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	// 써클은 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

	class CCircleComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CCircleComponent)

	public:
		using This = CCircleComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CCircleComponent(CObject* outer);
		CCircleComponent(CObject* outer, bool hasFace);

		virtual void ReadyToDrawing() override;

		glm::vec3 getNormal() const;
		virtual const SBound& GetBound() override;

		void Ctor();

	protected:
		bool hasFace_ = false;

		SCircleBound circleBound_;
	};
};
