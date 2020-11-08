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

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;

		virtual const class CCubeComponent* GetOBB() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

		float GetRadius() const;

	protected:
		CUniqPtr<TReflectionStruct> reflSphereCompo_;
	};
};
