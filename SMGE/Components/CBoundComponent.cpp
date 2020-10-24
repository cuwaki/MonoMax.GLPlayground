#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"
#include "../Assets/CAssetManager.h"

namespace SMGE
{
	SPlaneBound::SPlaneBound()
	{
		type_ = EBoundType::PLANE;
	}

	SCubeBound::SCubeBound()
	{
		type_ = EBoundType::CUBE;
	}

	CBoundComponent::CBoundComponent(CObject* outer) : CDrawComponent(outer)
	{
		Ctor();
	}

	void CBoundComponent::Ctor()
	{
		weakOBB_ = nullptr;

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

	SCubeBound CBoundComponent::GetAABB()
	{
		return GetOBB()->GetAABB();	// obb 즉 CCubeComponent 로부터 aabb 를 만든다, 정확도는 떨어질 수 있지만 빠르다, 사실 aabb 에서 정확도를 따지는 건 이미 무리인듯??
	}

	class CCubeComponent* CBoundComponent::CreateOBB()
	{
		auto map = FindOuter<CMap>(this);
		if (map != nullptr)	// 현재 맵이 없다면 beginplay 가 작동하지 않아서 제대로된 obb로서의 작동을 할 수 없다
		{
			getTransientComponents().push_back(MakeUniqPtr<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

			auto obbCube = DCast<CCubeComponent*>(getTransientComponents().back().get());
			if (obbCube)
			{
				// 여기 - LoadObject 와 같이 new 하고 reflect 까지 한번에 끝내주는 함수 필요
				auto asset = CAssetManager::LoadAsset<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
				obbCube->CopyFromTemplate(asset->getContentClass());

				// 여기 - new and register and beginplay 함수 필요 - 한방에 처리 되도록
				//{
				registerComponent(obbCube);

				if (map->IsStarted())
					obbCube->OnBeginPlay(this);
				// }

				return obbCube;
			}
		}

		return nullptr;
	}
};
