#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../Interfaces/CInt_Component.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	// nsRE::Transform 을 reflection 하기 위한 클래스이다
	// CActor 와 CDrawComponent 가 사용하는데 사용방법이 약간 다르다.
	// CDrawComponent 는 그 자신이 nsRE::Transform 이기 때문이다
	struct SGRefl_Transform : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans);

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		nsRE::Transform& nsre_transform_;
	};

	class CDrawComponent;

	struct SGRefl_DrawComponent : public SGRefl_Component
	{
		using Super = SGRefl_Component;
		using TReflectionClass = CDrawComponent;

		SGRefl_DrawComponent(TReflectionClass& rc);
		//SGRefl_DrawComponent(const CUniqPtr<CDrawComponent>& uptr);// { persistentComponentsREFL_ RTTI 필요 이슈

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		SGRefl_Transform& sg_transform_;
		mutable int32_t persistentComponentNumber_ = 0;

		TReflectionClass& outerDrawCompo_;
	};

	// 자식 컴포넌트를 가질 수 있어야하므로 CInt_Component 상속
	class CDrawComponent : public CComponent, public nsRE::WorldModel, public CInt_Component
	{
	public:
		using Super = CComponent;
		using TReflectionStruct = SGRefl_DrawComponent;

		friend struct TReflectionStruct;

	public:
		CDrawComponent(CObject* outer);

		virtual void Tick(float td) override;
		virtual void Render(float td);

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;

		virtual void ReadyToDrawing();

		// CInt_Component
		virtual ComponentVector& getPersistentComponents() override;
		virtual ComponentVector& getTransientComponents() override;
		virtual ComponentVectorWeak& getAllComponents() override;

	protected:
		class nsRE::CRenderingEngine* GetRenderingEngine();

		SGRefl_Transform sg_drawTransform_;

		// CInt_Component
		ComponentVector persistentComponents_;
		ComponentVector transientComponents_;
		ComponentVectorWeak allComponents_;
	};
};
