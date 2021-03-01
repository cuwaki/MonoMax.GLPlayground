#include "CSystemGame.h"

namespace SMGE
{
	CSystemGame::CSystemGame(class CEngineBase* engine) : CSystemBase(engine)
	{
	}

	void CSystemGame::OnLinkWithRenderingEngine()
	{
		CSystemBase::OnLinkWithRenderingEngine();
	}

	void CSystemGame::OnDestroyingGameEngine()
	{

	}

	bool CSystemGame::ProcessUserInput()
	{
		return true;
	}

	void CSystemGame::OnChangedSystemState(const CString& stateName)
	{

	}
};
