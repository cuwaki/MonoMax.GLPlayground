#pragma once

#include "GECommonIncludes.h"

namespace SMGE
{
	// for CSystemBase 계열과의 작동
	class CActorInterface
	{
	public:
		virtual bool AmIEditorActor() const { return false; };
	};
	
	class CActorInterfaceEditor : public CActorInterface
	{
	public:
		virtual bool AmIEditorActor() const override { return true; }
	};
};
