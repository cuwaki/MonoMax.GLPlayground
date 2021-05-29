#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CPointComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CPointComponent)

	public:
		using This = CPointComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CPointComponent(CObject* outer);

		virtual void ReadyToDrawing() override;

		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflPointCompo_;

		SPointBound pointBound_;
	};
};
