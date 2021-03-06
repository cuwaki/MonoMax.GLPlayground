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

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;

		void SetBoundDataComponent(float size, const glm::vec3& direction);

		virtual CCubeComponent* CreateOBB() override;
		virtual const SBound& GetBound() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

		float getRayLength() const;
		glm::vec3 getRayDirection() const;

	protected:
		std::unique_ptr<TReflectionStruct> reflRayCompo_;
		SSegmentBound segBound_;
	};
};
