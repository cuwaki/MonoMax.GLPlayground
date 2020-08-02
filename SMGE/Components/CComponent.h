#pragma once
#include "../Objects/CObject.h"
#include "../Interfaces/CInt_Reflection.h"

namespace SMGE
{
	class CComponent;

	struct SGRefl_Component : public SGReflection
	{
		using Super = SGReflection;
		using TReflectionClass = CComponent;

		SGRefl_Component(TReflectionClass& rc);
		SGRefl_Component(const CUniqPtr<CComponent>& sptr);
		virtual operator CWString() const override { return Super::operator CWString(); }
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override { return Super::operator=(in); }
	};

	class CComponent : public CObject, public CInt_Reflection
	{
	public:
		using Super = CObject;
		using TReflectionStruct = SGRefl_Component;

		friend struct TReflectionStruct;

	public:
		CComponent(CObject* outer);
		virtual ~CComponent() {}

		virtual void OnBeginPlay(class CActor* parent);
		virtual void OnEndPlay();

		// CInt_Reflection
		virtual CWString getClassName() override { return className_; }
		// 일부러 CInt_Reflection의 나머지는 구현 안함

	protected:
		class CActor* parentActor_;
	};
}
