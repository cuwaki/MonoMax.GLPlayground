#pragma once

#include "../SMGE/CGEGameBase.h"

namespace MonoMaxGraphics
{
	class CGGameDev : public CGEGameBase
	{
		using Super = CGEGameBase;

	public:
		CGGameDev();
		virtual ~CGGameDev();

		virtual void Tick(float) override;

	public:
		class CGEEngineBase* engine_;
	};
};
