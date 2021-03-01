#pragma once

#include "CSystemBase.h"

namespace SMGE
{
	class CSystemGame : public CSystemBase
	{
	public:
		CSystemGame(class CEngineBase* engine);

		virtual void OnLinkWithRenderingEngine() override;
		virtual void OnDestroyingGameEngine() override;

		virtual bool ProcessUserInput() override;
		virtual void OnChangedSystemState(const CString& stateName) override;
	};
};
