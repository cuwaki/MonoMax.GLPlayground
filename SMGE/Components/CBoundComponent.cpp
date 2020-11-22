#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"
#include "../Assets/CAssetManager.h"

namespace SMGE
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

		CacheAABB();
	}

	void CBoundComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	bool CBoundComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}

	void CBoundComponent::CacheAABB()
	{
		const_cast<CCubeComponent*>(GetOBB())->CacheAABB();	// obb 즉 CCubeComponent 로부터 aabb 를 만든다, 정확도는 떨어질 수 있지만 빠르다, 사실 aabb 에서 정확도를 따지는 건 이미 무리인듯??
		cachedAABB_ = GetOBB()->GetAABB();
	}

	const SAABB& CBoundComponent::GetAABB() const
	{
		return cachedAABB_;
	}

	const class CCubeComponent* CBoundComponent::GetOBB() const
	{
		return weakOBB_;	// const 객체의 경우 아직 캐시가 안되었을 때 올수도 있다, 좀 생각해보자! 애초부터 const 가 아니었으면 const 가 아닌 GetOBB() 가 실행되었을테니까 음...
	}

	class CCubeComponent* CBoundComponent::CreateOBB()
	{
		auto map = FindOuter<CMap>(this);
		if (map != nullptr)	// 현재 맵이 없다면 beginplay 가 작동하지 않아서 제대로된 obb로서의 작동을 할 수 없다
		{
			// 이렇게 단위 크기의 큐브콤포넌트를 만들어서 자식 콤포넌트로 붙이면 현재의 루트객체의 트랜스폼을 따라서 자동으로 트랜스폼 조정이 되게 된다.
			// 점, 레이 같이 몇몇 특수한 케이스 외에는 추가 처리가 필요없다.

			getTransientComponents().push_back(MakeUniqPtr<CCubeComponent>(RTTI_CObject::NewDefault<CCubeComponent>(this)));

			auto obbCube = DCast<CCubeComponent*>(getTransientComponents().back().get());
			if (obbCube)
			{
				// 일단 안보이게
				obbCube->isGameVisible_ = false;
#if IS_EDITOR
				obbCube->isEditorVisible_ = false;
#endif

				// 여기 - LoadObject 와 같이 new 하고 reflect 까지 한번에 끝내주는 함수 필요
				auto asset = CAssetManager::LoadAsset<CComponent>(Globals::GetGameAssetPath(wtext("/templates/CCubeComponent.asset")));
				obbCube->CopyFromTemplate(asset->getContentClass());

				// 여기 - new and register and beginplay 함수 필요 - 한방에 처리 되도록
				//{
				registerComponent(obbCube);

				if (map->IsBeganPlay())
					obbCube->OnBeginPlay(this);
				// }

				return obbCube;
			}
		}

		return nullptr;
	}
};
