#include "CGObject.h"

namespace MonoMaxGraphics
{
	CGObject::CGObject()
	{
		reflClassName_ = wtext("SMGE_Game::CGObject");
	}

	void CGObject::CGCtor()
	{
		// SMGE_Game �ý����� ���� ������
	}

	void CGObject::CopyFromTemplate(const CGObject& templateObj)
	{
	}
}
