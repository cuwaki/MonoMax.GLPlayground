#include "CComponent.h"

namespace SMGE
{
	void CComponent::OnBeginPlay(class CActor* parent)
	{
		parentActor_ = parent;
	}

	void CComponent::OnEndPlay()
	{
		parentActor_ = nullptr;
	}
};
