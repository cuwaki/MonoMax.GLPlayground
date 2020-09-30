#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CSphereComponent;

	struct SGRefl_SphereComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CSphereComponent;

		SGRefl_SphereComponent(TReflectionClass& rc);

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		float& radius_;
		SGRefl_Transform sg_transform_;
		TReflectionClass& outerSphereCompo_;
	};

	class CSphereComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CSphereComponent)

	public:
		using Super = CBoundComponent;
		using TReflectionStruct = SGRefl_SphereComponent;

		friend struct TReflectionStruct;

	public:
		CSphereComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

		float GetRadius() const { return radius_; }

	protected:
		CUniqPtr<TReflectionStruct> reflSphereCompo_;
		float radius_;
	};
};
