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

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

		nsRE::Transform& nsre_transform_;
	};

	class CDrawComponent;

	struct SGRefl_DrawComponent : public SGRefl_Component
	{
		using Super = SGRefl_Component;
		using TReflectionClass = CDrawComponent;

		SGRefl_DrawComponent(TReflectionClass& rc);

		virtual const SGReflection& operator>>(CWString& out) const override;
		virtual SGReflection& operator<<(const CVector<TupleVarName_VarType_Value>& in) override;

		SGRefl_Transform sg_transform_;
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

		void Ctor();

		virtual void Tick(float td) override;

		virtual void OnBeginPlay(class CObject* parent) override;
		virtual void OnEndPlay() override;

		virtual void ReadyToDrawing();

		bool IsGameRendering() const;
		void SetGameRendering(bool ir);
#if IS_EDITOR
		bool IsEditorRendering() const;
		void SetEditorRendering(bool ir);
#endif

		// CInt_Component
		virtual ComponentVector& getPersistentComponents() override;
		virtual ComponentVector& getTransientComponents() override;
		virtual ComponentVectorWeak& getAllComponents() override;

		class CBoundComponent* GetMainBound();

	protected:
		// CInt_Component
		ComponentVector persistentComponents_;
		ComponentVector transientComponents_;
		ComponentVectorWeak allComponents_;
		mutable class CBoundComponent* mainBoundCompo_;

		bool isGameRendering_;
#if IS_EDITOR
		bool isEditorRendering_;
#endif
	};
};
