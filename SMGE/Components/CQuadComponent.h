#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	// 쿼드는 X 와 Y 로만 만들어져야한다, Z 는 Configs::BoundEpsilon 로 고정이거나 마치 0처럼 취급될 것이다

	class CQuadComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CQuadComponent)

	public:
		using This = CQuadComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CQuadComponent(CObject* outer);
		CQuadComponent(CObject* outer, bool hasFace);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint) override;

		glm::vec3 getNormal() const;
		virtual const SBound& getBound() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		bool hasFace_ = false;
		CUniqPtr<TReflectionStruct> reflPlaneCompo_;

		SQuadBound quadBound_;
	};
};
