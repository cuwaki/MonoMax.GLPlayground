#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
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
		SGRefl_DrawComponent(const CUniqPtr<CDrawComponent>& uptr);
		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;

		SGRefl_Transform& sg_transform_;
	};

	class CDrawComponent : public CComponent, public nsRE::WorldModel
	{
	public:
		using Super = CComponent;
		using TReflectionStruct = SGRefl_DrawComponent;

		friend struct TReflectionStruct;

	public:
		CDrawComponent(CObject* outer);

		virtual void Tick(float td);
		virtual void Render(float td);

		virtual void OnBeginPlay(class CActor* parent) override;
		virtual void OnEndPlay() override;

		virtual void ReadyToDrawing();

	protected:
		class nsRE::CRenderingEngine* GetRenderingEngine();

		SGRefl_Transform sg_drawTransform_;
	};
};
