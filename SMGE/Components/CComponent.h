#pragma once
#include "../Objects/CObject.h"

namespace SMGE
{
	class CComponent : public CObject
	{
	public:
		CComponent(CObject* outer) : CObject(outer) {};

		virtual ~CComponent() {}

		virtual void OnBeginPlay(class CActor* parent);
		virtual void OnEndPlay();

	protected:
		class CActor* parentActor_;
	};
}
