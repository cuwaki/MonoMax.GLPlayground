#include "CGizmoActor.h"

namespace SMGE
{
	CGizmoActor::CGizmoActor(CObject* outer) : Super(outer)
	{
		Ctor();
	}

	void CGizmoActor::Ctor()
	{
		//auto meshCompo = std::make_unique<CMeshComponent>(this);
		//getPersistentComponents().emplace_back(std::move(meshCompo));
	}

	void CGizmoActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	CGizmoActorTranslate::CGizmoActorTranslate(CObject* outer) : CGizmoActor(outer)
	{
	}
}
