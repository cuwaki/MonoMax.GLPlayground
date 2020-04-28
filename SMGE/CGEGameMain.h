#pragma once

#include "GECommonIncludes.h"

namespace MonoMaxGraphics
{
	struct SGEGameSettings
	{
		CWString gameProjectName_;
	};

	class CGEGameMain
	{
	public:
		CGEGameMain();

		virtual void Tick(float timeDelta);

	public:
		SGEGameSettings* gameSettings_;
	};
};
