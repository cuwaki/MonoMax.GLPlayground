#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CSegmentComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CSegmentComponent)

	public:
		using This = CSegmentComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CSegmentComponent(CObject* outer);

		virtual void ReadyToDrawing() override;

		void SetBoundLocalSpace(float size, const glm::vec3& direction);

		virtual CCubeComponent* CreateOBB() override;

		float getLength(bool isWorld) const;
		glm::vec3 getDirection(bool isWorld) const;

		virtual const SBound& GetBoundWorldSpace(bool isForceRecalc = false) override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflRayCompo_;
		SSegmentBound segBound_;
	};
};
