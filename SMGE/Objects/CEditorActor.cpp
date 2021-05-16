#include "CEditorActor.h"

namespace SMGE
{
	CEditorActor::CEditorActor(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CEditorActor::Ctor()
	{
		//auto meshCompo = std::make_unique<CMeshComponent>(this);
		//getPersistentComponents().emplace_back(std::move(meshCompo));
	}

	void CEditorActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	CGizmoActorTransform::CGizmoActorTransform(CObject* outer) : CEditorActor(outer)
	{
	}
}
