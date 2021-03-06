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

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;

		virtual const SBound& GetBound() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		std::unique_ptr<TReflectionStruct> reflCubeCompo_;

		SCubeBound cubeBound_;
	};
};
