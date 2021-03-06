#include "CComponent.h"

namespace SMGE
{
	SGRefl_Component::SGRefl_Component(TReflectionClass& rc) : Super(rc)
	{
	}
	//SGRefl_Component::SGRefl_Component(const std::unique_ptr<CComponent>& uptr) : Super(*uptr.get())
	//{
	//}

	CComponent::CComponent(CObject* outer) : CObject(outer)
	{
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
			reflCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflCompo_.get();
	}
};
