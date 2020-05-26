#include "CObject.h"

namespace SMGE
{
	CObject::CObject()
	{
		className_ = wtext("SMGE_Game::CObject");
	}

	void CObject::CGCtor()
	{
		// SMGE_Game 시스템을 위한 생성자	
	}
}
