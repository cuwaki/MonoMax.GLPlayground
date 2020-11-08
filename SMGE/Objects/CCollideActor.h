#pragma once

#include "CActor.h"

namespace SMGE
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CCollideActor : public CActor
	{
		DECLARE_RTTI_CObject(CCollideActor)

		using Super = CActor;

	public:
		CCollideActor(CObject* outer) : Super(outer) {}
		CCollideActor(CObject* outer, ECheckCollideRule rule, bool isPolygonCheck, const DELEGATE_OnCollide& fOnCollide);

		virtual CVector<CActor*> QueryCollideCheckTargets();
		virtual void ProcessCollide(ECheckCollideRule rule, bool isPolygonCheck, const DELEGATE_OnCollide& fOnCollide, CVector<CActor*>& targets);
		virtual void ProcessCollide(CVector<CActor*>& targets);

	protected:
		ECheckCollideRule rule_;
		bool isPolygonCheck_;
		DELEGATE_OnCollide fOnCollide_;
	};
}
