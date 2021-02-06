#pragma once

#include "CSystemBase.h"
#include <forward_list>

namespace SMGE
{
	class CSystemEditor : public CSystemBase
	{
	public:
		CSystemEditor(class CEngineBase* engine);

		virtual void OnStartSystem() override;
		virtual void OnEndSystem() override;

		virtual void OnChangedSystemState(const CString& stateName) override;

	protected:
		std::forward_list<class CObject*> selectedObjects_;
	};
};
