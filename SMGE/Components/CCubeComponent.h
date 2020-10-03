#pragma once

#include "../GECommonIncludes.h"
#include "CBoundComponent.h"

namespace SMGE
{
	class CCubeComponent;

	struct SGRefl_CubeComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CCubeComponent;

		SGRefl_CubeComponent(TReflectionClass& rc);

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		glm::vec3& centerPos_;
		glm::vec3& size_;
		SGRefl_Transform sg_transform_;
		TReflectionClass& outerCubeCompo_;
	};

	class CCubeComponent : public CBoundComponent
	{
		DECLARE_RTTI_CObject(CCubeComponent)

	public:
		using This = CCubeComponent;
		using Super = CBoundComponent;
		using TReflectionStruct = SGRefl_CubeComponent;

		friend struct TReflectionStruct;

	public:
		CCubeComponent(CObject* outer);
		CCubeComponent(CObject* outer, const glm::vec3& leftBottom, const glm::vec3& rightTop);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;
		virtual void ReadyToDrawing() override;
		virtual bool CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint) override;

		virtual class CCubeComponent* GetOBB() override;
		virtual class CCubeComponent* GetAABB() override;

		void Ctor();

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflCubeCompo_;
		glm::vec3 centerPos_;
		glm::vec3 size_;
	};
};
