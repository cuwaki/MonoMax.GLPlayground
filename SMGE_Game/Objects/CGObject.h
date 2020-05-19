#pragma once

#include "../GCommonIncludes.h"

namespace MonoMaxGraphics
{
	class CGObject
	{
		friend struct SGReflection;

	public:
		CGObject();

		CWString getClassName() { return className_; }

		virtual void CGCtor();

	protected:
		CWString className_;
	};
};
