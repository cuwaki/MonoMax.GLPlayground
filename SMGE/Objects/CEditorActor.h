#pragma once

#include "CActor.h"

namespace SMGE
{
	class CEditorActor : public CActor
	{
		//DECLARE_RTTI_CObject(CEditorActor)

		using Super = CActor;

	public:
		CEditorActor(CObject* outer);
		void Ctor();

		virtual void BeginPlay() override;

	protected:
		CActor* linkedActorW_ = nullptr;
	};

	class CGizmoActorTransform : public CEditorActor
	{
		DECLARE_RTTI_CObject(CGizmoActorTransform)

		using Super = CEditorActor;

	public:
		CGizmoActorTransform(CObject* outer);
	};
}
