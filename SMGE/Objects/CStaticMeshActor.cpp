#include "CStaticMeshActor.h"
#include "../Components/CMeshComponent.h"
#include "../Components/CSphereComponent.h"
#include "../Components/CMovementComponent.h"

namespace SMGE
{
	CStaticMeshActor::CStaticMeshActor(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CStaticMeshActor::Ctor()
	{
		auto meshCompo = MakeUniqPtr<CMeshComponent>(this);
		getPersistentComponents().emplace_back(std::move(meshCompo));
	}

	void CStaticMeshActor::BeginPlay()
	{
		Super::BeginPlay();

		// 테스트 코드
		auto moveCompo = MakeUniqPtr<CMovementComponent>(this);
		getTransientComponents().emplace_back(std::move(moveCompo));

		GetMainBound()->GetOBB();
	}
}
