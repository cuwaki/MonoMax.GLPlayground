#pragma once

#include "CActor.h"

namespace SMGE
{
	class CGizmoActor : public CActor
	{
		//DECLARE_RTTI_CObject(CGizmoActor)

		using Super = CActor;

	public:
		CGizmoActor(CObject* outer);
		void Ctor();

		virtual void BeginPlay() override;

	protected:
		CActor* linkedActorW_ = nullptr;

	public:
		virtual bool AmIEditorActor() const { return true; };
	};

	class CGizmoActorTranslate : public CGizmoActor
	{
		DECLARE_RTTI_CObject(CGizmoActorTranslate)

		using Super = CGizmoActor;

	public:
		CGizmoActorTranslate(CObject* outer);
	};
}
