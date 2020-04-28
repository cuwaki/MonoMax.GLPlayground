#pragma once

#include "../GCommonIncludes.h"

namespace MonoMaxGraphics
{
	class CGObject
	{
		friend struct SGReflection;

	public:
		CGObject();

		virtual void MakeDefault();

	protected:
		CWString reflClassName_;
	};
};
