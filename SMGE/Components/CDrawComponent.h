#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../Interfaces/CInt_Component.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	// nsRE::Transform �� reflection �ϱ� ���� Ŭ�����̴�
	// CActor �� CDrawComponent �� ����ϴµ� ������� �ణ �ٸ���.
	// CDrawComponent �� �� �ڽ��� nsRE::Transform �̱� �����̴�
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

		virtual void OnBeforeSerialize() const override;
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		SGRefl_Transform sg_transform_;
		TReflectionClass& outerDrawCompo_;
	};

	// �ڽ� ������Ʈ�� ���� �� �־���ϹǷ� CInt_Component ���
	class CDrawComponent : public CComponent, public nsRE::WorldObject, public CInt_Component
	{
	public:
		using Super = CComponent;
		using TReflectionStruct = SGRefl_DrawComponent;

		friend struct TReflectionStruct;

	public:
		CDrawComponent(CObject* outer);

		void Ctor();

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

		// CInt_Component
		ComponentVector persistentComponents_;
		ComponentVector transientComponents_;
		ComponentVectorWeak allComponents_;

		bool isGameVisible_;
#if IS_EDITOR
		bool isEditorVisible_;
#endif
	};
};
