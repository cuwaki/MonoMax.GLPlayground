#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CRayComponent;

	struct SGRefl_RayComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CRayComponent;

		SGRefl_RayComponent(TReflectionClass& rc);
		//SGRefl_RayComponent(const CUniqPtr<CRayComponent>& uptr);// { persistentComponentsREFL_ RTTI « ø‰ ¿ÃΩ¥

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		float& size_;
		glm::vec3& direction_;
		SGRefl_Transform sg_transform_;
		TReflectionClass& outerRayCompo_;
	};

	class CRayComponent : public CBoundComponent
	{
	public:
		using Super = CBoundComponent;
		using TReflectionStruct = SGRefl_RayComponent;

		friend struct TReflectionStruct;

	public:
		CRayComponent(CObject* outer, float size, const glm::vec3& direction);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint) override;

		void Ctor();

		// CInt_Reflection
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflRayCompo_;
		float size_;
		glm::vec3 direction_;
	};
};
