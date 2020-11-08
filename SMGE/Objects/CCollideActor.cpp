#include "CCollideActor.h"
#include "../Components/CPointComponent.h"
#include "../Components/CRayComponent.h"
#include "../Components/CCubeComponent.h"
#include "../Components/CSphereComponent.h"
#include "../CGameBase.h"
#include "CMap.h"

namespace SMGE
{
	CCollideActor::CCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide) : CActor(outer), fOnCollide_(fOnCollide)
	{
		rule_ = rule;
		isDetailCheck_ = isDetailCheck;
	}

	CVector<CActor*> CCollideActor::QueryCollideCheckTargets()
	{
		auto aabb = GetMainBound()->GetAABB();

		CVector<CActor*> ret;
		ret.reserve(10);

		auto actorWeaks = Globals::GCurrentMap->QueryActors(aabb);
		for (size_t i = 0; i < actorWeaks.size(); ++i)
		{
			if (actorWeaks[i] != nullptr && actorWeaks[i]->GetMainBound() != nullptr && actorWeaks[i] != this)
				ret.push_back(actorWeaks[i]);
		}

		return ret;
	}


	void CCollideActor::ProcessCollide(CVector<CActor*>& targets)
	{
		ProcessCollide(rule_, isDetailCheck_, fOnCollide_, targets);
	}

	void CCollideActor::ProcessCollide(ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, CVector<CActor*>& targets)
	{
		glm::vec3 collidingPoint;

		for (auto& actor : targets)
		{
			if (this->GetMainBound()->CheckCollide(actor->GetMainBound(), collidingPoint) == true)
			{
				if (isDetailCheck == false)
				{
					fOnCollide(this, actor, this->GetMainBound(), actor->GetMainBound(), collidingPoint);
				}
				else
				{
					// 디테일 바운드 콤포에 대하여 체크 - 여러개의 추가 Bound 또는 디테일한 CPolygonComponent 가 있어야겠지?
					assert(false);	// 아직 미지원
				}
			}
		}
	}
}
