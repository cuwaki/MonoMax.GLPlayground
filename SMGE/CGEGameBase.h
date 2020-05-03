#pragma once

#include "GECommonIncludes.h"

namespace MonoMaxGraphics
{
	struct SGEGameSettings
	{
		CWString gameProjectName_;
	};

	class CGEGameBase
	{
	public:
		CGEGameBase();
		virtual ~CGEGameBase();

		virtual void Tick(float timeDelta);

	public:
		SGEGameSettings* gameSettings_;
	};
};
