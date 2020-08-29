#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"

namespace SMGE
{
	CBoundComponent::CBoundComponent(CObject* outer) : CDrawComponent(outer)
	{
		className_ = wtext("SMGE::CBoundComponent");

		Ctor();
	}

	void CBoundComponent::Ctor()
	{
		isPickingTarget_ = true;
		isCollideTarget_ = true;
	}

	void CBoundComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CBoundComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}
};
