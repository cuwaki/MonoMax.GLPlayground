#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CCubeComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CCubeComponent)

	public:
		using This = CCubeComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CCubeComponent(CObject* outer);

		virtual void ReadyToDrawing() override;
		virtual class CCubeComponent* CreateOBB() override { return this; }
		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		std::unique_ptr<TReflectionStruct> reflCubeCompo_;

		SCubeBound cubeBound_;
	};
};
