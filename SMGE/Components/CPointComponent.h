#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CPointComponent;

	struct SGRefl_PointComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CPointComponent;

		SGRefl_PointComponent(TReflectionClass& rc);
		//SGRefl_PointComponent(const CUniqPtr<CPointComponent>& uptr);// { persistentComponentsREFL_ RTTI « ø‰ ¿ÃΩ¥

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		SGRefl_Transform sg_transform_;
		TReflectionClass& outerPointCompo_;
	};

	class CPointComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CPointComponent)

	public:
		using Super = CBoundComponent;
		using TReflectionStruct = SGRefl_PointComponent;

		friend struct TReflectionStruct;

	public:
		CPointComponent(CObject* outer);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint) override;

		void Ctor();

		// CInt_Reflection
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflPointCompo_;
	};
};
