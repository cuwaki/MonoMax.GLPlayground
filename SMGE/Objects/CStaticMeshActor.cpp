#include "CStaticMeshActor.h"
#include "../Components/CMeshComponent.h"
#include "../Components/CSphereComponent.h"

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

		// 테스트 코드 - 레이컴포넌트의 obb 생성하기
		GetMainBound()->GetOBB();
	}
}
