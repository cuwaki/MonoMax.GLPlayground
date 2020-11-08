#include "CCollideActor.h"
#include "../Components/CPointComponent.h"
#include "../Components/CRayComponent.h"
#include "../Components/CCubeComponent.h"
#include "../Components/CSphereComponent.h"
#include "../CGameBase.h"
#include "CMap.h"

namespace SMGE
{
	CCollideActor::CCollideActor(CObject* outer, ECheckCollideRule rule, bool isPolygonCheck, const DELEGATE_OnCollide& fOnCollide) : CActor(outer), fOnCollide_(fOnCollide)
	{
		rule_ = rule;
		isPolygonCheck_ = isPolygonCheck;
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
		ProcessCollide(rule_, isPolygonCheck_, fOnCollide_, targets);
	}

	void CCollideActor::ProcessCollide(ECheckCollideRule rule, bool isPolygonCheck, const DELEGATE_OnCollide& fOnCollide, CVector<CActor*>& targets)
	{
		glm::vec3 collidingPoint;

		for (auto& actor : targets)
		{
			if (
				//this->GetMainBound()->GetAABB().isIntersect(actor->GetMainBound()->GetAABB()) == true &&	// aabb 체크 넘어가면
				this->GetMainBound()->CheckCollide(actor->GetMainBound(), collidingPoint) == true)			// 바운드끼리 체크 한다
			{
				if (isPolygonCheck == false)
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
