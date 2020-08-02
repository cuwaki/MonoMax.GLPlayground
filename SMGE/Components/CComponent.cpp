#include "CComponent.h"

namespace SMGE
{
	SGRefl_Component::SGRefl_Component(TReflectionClass& rc) : Super(rc)
	{
	}
	SGRefl_Component::SGRefl_Component(const CUniqPtr<CComponent>& uptr) : Super(*uptr.get())
	{
	}

	CComponent::CComponent(CObject* outer) : CObject(outer)
	{
		className_ = wtext("SMGE::CComponent");
	}

	void CComponent::OnBeginPlay(class CActor* parent)
	{
		parentActor_ = parent;
	}

	void CComponent::OnEndPlay()
	{
		parentActor_ = nullptr;
	}
};
