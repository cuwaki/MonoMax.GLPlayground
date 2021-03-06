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

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;

		virtual const SBound& GetBound() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		std::unique_ptr<TReflectionStruct> reflPointCompo_;

		SPointBound pointBound_;
	};
};
