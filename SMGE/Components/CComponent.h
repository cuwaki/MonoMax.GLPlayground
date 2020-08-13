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
		//SGRefl_Component(const CUniqPtr<CComponent>& sptr);	// { persistentComponentsREFL_ RTTI 필요 이슈
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

		virtual void OnBeginPlay(class CObject* parent);
		virtual void OnEndPlay();
		virtual void Tick(float td) {}

		void setActive(bool isa) { isActive_ = isa; }
		bool isActive() const { return isActive_; }

		// CInt_Reflection
		virtual const CWString& getClassName() override { return className_; }
		// 일부러 CInt_Reflection의 나머지는 구현 안함

	protected:
		bool isActive_ = false;
	};
}
