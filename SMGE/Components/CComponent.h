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
	};

	class CComponent : public CObject, public CInt_Reflection
	{
		DECLARE_RTTI_CObject(CComponent)

	public:
		using This = CComponent;
		using Super = CObject;
		using TReflectionStruct = SGRefl_Component;

		friend struct TReflectionStruct;

	public:
		CComponent(CObject* outer);
		virtual ~CComponent() {}

		virtual void OnBeginPlay(class CObject* parent);
		virtual void OnEndPlay();
		virtual void Tick(float) {}
		virtual void AfterTick(float) {}

		void setActive(bool isa) { isActive_ = isa; }
		bool isActive() const { return isActive_; }

		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;

	protected:
		UPtr<TReflectionStruct> reflCompo_;
		bool isActive_ = false;
	};
}
