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
		virtual operator CWString() const override { return Super::operator CWString(); }
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override { return Super::operator=(in); }
	};

	class CComponent : public CObject, public CInt_Reflection
	{
		DECLARE_RTTI_CObject(CComponent)

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
		virtual const CString& getClassRTTIName() const override { return GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;
		virtual void OnAfterDeserialized() override {}

	protected:
		CUniqPtr<TReflectionStruct> reflCompo_;
		bool isActive_ = false;
	};
}
