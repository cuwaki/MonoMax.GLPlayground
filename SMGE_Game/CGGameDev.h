#pragma once

#include "../SMGE/CGEGameMain.h"

namespace MonoMaxGraphics
{
	class CGGameDev : public CGEGameMain
	{
		using Super = CGEGameMain;

	public:
		CGGameDev() {}

		virtual void Tick(float) override;
	};
};
