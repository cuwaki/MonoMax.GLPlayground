#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CSphereComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CSphereComponent)

	public:
		using This = CSphereComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CSphereComponent(CObject* outer);

		virtual void ReadyToDrawing() override;

		float GetRadius(bool isWorld) const;
		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflSphereCompo_;
		SSphereBound sphereBound_;
	};
};
