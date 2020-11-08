#include "CBoundComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "CCubeComponent.h"
#include "../Objects/CMap.h"
#include "../Assets/CAssetManager.h"

namespace SMGE
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SPlaneBound::SPlaneBound()
	{
		type_ = EBoundType::PLANE;
	}

	SCubeBound::SCubeBound()
	{
		type_ = EBoundType::CUBE;
	}

	bool SAABB::isIntersect(const SAABB& other) const
	{
		if (isIntersectPoints(other))	// 점이 포함된 경우 또는 this가 other 를 완전히 감싼 경우
			return true;

		if (other.isIntersectPoints(*this))	// other 가 this 를 완전히 감싼 경우
			return true;

		// 점이 포함이 아니고 선분이 포함인 경우의 체크 : + 이런 식으로 크로스 된 경우
		bool xyCross = (isXContains(other) && other.isYContains(*this)) || other.isXContains(*this) && this->isYContains(other);
		if (xyCross == true)
		{	// xy 로는 크로스 관계임이 확실해짐

			// zx 로 크로스인지 체크
			bool zxCross = (isZContains(other) && other.isXContains(*this)) || other.isZContains(*this) && this->isXContains(other);
			return zxCross;
		}

		return false;
	}

	bool SAABB::isXContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.x >= lb_.x && otherLB.x < rt_.x && otherRT.x >= lb_.x && otherRT.x < rt_.x;
	}
	bool SAABB::isYContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.y >= lb_.y && otherLB.y < rt_.y&& otherRT.y >= lb_.y && otherRT.y < rt_.y;
	}
	bool SAABB::isZContains(const SAABB& other) const
	{
		const auto otherLB = other.lb(), otherRT = other.rt();
		return otherLB.z >= lb_.z && otherLB.z < rt_.z&& otherRT.z >= lb_.z && otherRT.z < rt_.z;
	}
	bool SAABB::isIntersectPoints(const SAABB& other) const
	{
		return this->isContains(other.lb()) || this->isContains(other.rt()) || this->isContains(other.lt()) || this->isContains(other.rb());
	}

	inline bool SAABB::isContains(const glm::vec3& point) const
	{
		return
			point.x >= lb_.x && point.x < rt_.x&&
			point.y >= lb_.y && point.y < rt_.y&&
			point.z >= lb_.z && point.z < rt_.z;
	}

	inline const glm::vec3& SAABB::lb() const
	{
		return lb_;
	}
	inline const glm::vec3& SAABB::rt() const
	{
		return rt_;
	}
	inline glm::vec3 SAABB::lt() const
	{
		return { lb_.x, rt_.y, lb_.z };
	}	
	inline glm::vec3 SAABB::rb() const
	{
		return { rt_.x, lb_.y, rt_.z };
	}

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
