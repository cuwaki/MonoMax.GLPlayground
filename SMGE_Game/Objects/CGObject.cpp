#include "CGObject.h"

namespace MonoMaxGraphics
{
	CGObject::CGObject()
	{
		reflClassName_ = wtext("SMGE_Game::CGObject");
	}

	void CGObject::CGCtor()
	{
		// SMGE_Game 시스템을 위한 생성자
	}

	void CGObject::CopyFromTemplate(const CGObject& templateObj)
	{
	}
}
