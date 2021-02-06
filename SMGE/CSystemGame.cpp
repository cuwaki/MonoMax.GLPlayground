#include "CSystemGame.h"

namespace SMGE
{
	CSystemGame::CSystemGame(class CEngineBase* engine) : CSystemBase(engine)
	{
	}

	void CSystemGame::OnStartSystem()
	{
	}

	void CSystemGame::OnEndSystem()
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
