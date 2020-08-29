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
		//SGRefl_SphereComponent(const CUniqPtr<CSphereComponent>& uptr);// { persistentComponentsREFL_ RTTI « ø‰ ¿ÃΩ¥

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		float& radius_;
		SGRefl_Transform sg_transform_;
		TReflectionClass& outerSphereCompo_;
	};

	class CSphereComponent : public CBoundComponent
	{
	public:
		using Super = CBoundComponent;
		using TReflectionStruct = SGRefl_SphereComponent;

		friend struct TReflectionStruct;

	public:
		CSphereComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;

		void Ctor();

		// CInt_Reflection
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflSphereCompo_;
		float radius_;
	};
};
