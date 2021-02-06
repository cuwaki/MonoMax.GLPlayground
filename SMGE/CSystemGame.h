#pragma once

#include "CSystemBase.h"

namespace SMGE
{
	class CSystemGame : public CSystemBase
	{
	public:
		CSystemGame(class CEngineBase* engine);

		virtual void OnStartSystem() override;
		virtual void OnEndSystem() override;

		virtual bool ProcessUserInput() override;
		virtual void OnChangedSystemState(const CString& stateName) override;
	};
};
