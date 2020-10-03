#pragma once

#include "CActor.h"

namespace SMGE
{
	class CStaticMeshActor : public CActor
	{
		DECLARE_RTTI_CObject(CStaticMeshActor)

		using Super = CActor;

	public:
		CStaticMeshActor(CObject* outer);
		void Ctor();

		virtual void BeginPlay() override;
	};
}
