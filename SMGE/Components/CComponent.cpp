#include "CComponent.h"

namespace SMGE
{
	SGRefl_Component::SGRefl_Component(TReflectionClass& rc) : Super(rc)
	{
	}
	//SGRefl_Component::SGRefl_Component(const CUniqPtr<CComponent>& uptr) : Super(*uptr.get())
	//{
	//}

	CComponent::CComponent(CObject* outer) : CObject(outer)
	{
		//classRTTIName_ = "SMGE::CComponent";
	}

	void CComponent::OnBeginPlay(class CObject* parent)
	{
	}

	void CComponent::OnEndPlay()
	{
	}

	SGReflection& CComponent::getReflection()
	{
		if (reflCompo_.get() == nullptr)
			reflCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflCompo_.get();
	}
};
