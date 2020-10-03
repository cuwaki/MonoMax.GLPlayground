#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"

namespace SMGE
{
	CBoundComponent::CBoundComponent(CObject* outer) : CDrawComponent(outer)
	{
		Ctor();
	}

	void CBoundComponent::Ctor()
	{
		cachedOBB_ = nullptr;

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

	bool CBoundComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	class CCubeComponent* CBoundComponent::GetAABB()
	{
		return GetOBB()->GetAABB();	// obb 즉 CCubeComponent 로부터 aabb 를 만든다, 정확도는 떨어질 수 있지만 빠르다, 사실 aabb 에서 정확도를 따지는 건 이미 무리인듯??
	}

	class CCubeComponent* CBoundComponent::CreateOBB(const glm::vec3& lb, const glm::vec3& rt)
	{
		auto map = FindOuter<CMap>(this);
		if (map != nullptr)	// 현재 맵이 없다면 beginplay 가 작동하지 않아서 제대로된 obb로서의 작동을 할 수 없다
		{
			getTransientComponents().push_back(MakeUniqPtr<CCubeComponent>(RTTI_CObject::NewVariety<CCubeComponent>(this, lb, rt)));

			auto cube = static_cast<CCubeComponent*>(getTransientComponents().back().get());

			if (map->IsStarted())
				cube->OnBeginPlay(this);

			return cube;
		}

		return nullptr;
	}
};
