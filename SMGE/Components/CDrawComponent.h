#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
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
