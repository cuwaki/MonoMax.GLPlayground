#pragma once

#include "../GCommonIncludes.h"

namespace MonoMaxGraphics
{
	class CGObject
	{
		friend struct SGReflection;

	public:
		CGObject();

		CWString getReflClassName() { return reflClassName_; }

		virtual void CGCtor();
		virtual void CopyFromTemplate(const CGObject& templateObj);

	protected:
		CWString reflClassName_;
	};
};
