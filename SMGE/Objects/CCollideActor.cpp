#include "CCollideActor.h"
#include "../Components/CPointComponent.h"
#include "../Components/CSegmentComponent.h"
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


	void CCollideActor::CheckCollideAndProcess(CVector<CActor*>& targets)
	{
		CheckCollideAndProcess(rule_, isPolygonCheck_, fOnCollide_, targets);
	}

	void CCollideActor::CheckCollideAndProcess(ECheckCollideRule rule, bool isPolygonCheck, const DELEGATE_OnCollide& fOnCollide, CVector<CActor*>& targets)
	{
		SSegmentBound crossSeg;

		auto thisMainBound = this->GetMainBound();

		for (auto& actor : targets)
		{
			auto otherMainBound = actor->GetMainBound();
			if (thisMainBound->GetAABB().check(otherMainBound->GetAABB(), crossSeg) == true &&	// aabb 체크 넘어가면
				thisMainBound->GetBound().check(otherMainBound->GetBound(), crossSeg) == true)	// 바운드끼리 체크 한다
			{
				if (isPolygonCheck == false)
				{
					fOnCollide(this, actor, thisMainBound, otherMainBound, crossSeg);
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
