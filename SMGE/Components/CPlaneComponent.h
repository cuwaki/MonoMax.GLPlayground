#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CPlaneComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CPlaneComponent)

	public:
		using This = CPlaneComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = typename Super::TReflectionStruct;

		friend struct TReflectionStruct;

	public:
		CPlaneComponent(CObject* outer);
		CPlaneComponent(CObject* outer, bool hasFace);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint) override;

		virtual class CCubeComponent* GetOBB() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	public:
		glm::vec3 getNormal() const;
		SPlaneBound getBound() const;

	protected:
		bool hasFace_ = false;
		CUniqPtr<TReflectionStruct> reflPlaneCompo_;
	};
};
