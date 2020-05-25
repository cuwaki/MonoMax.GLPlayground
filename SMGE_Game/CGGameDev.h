#pragma once

#include "../SMGE/CGEGameBase.h"

#define IS_EDITOR 1
#define IS_GAME !(defined IS_EDITOR)

namespace SMGE
{
	class CGGameDev : public CGEGameBase
	{
		using Super = CGEGameBase;

	public:
		CGGameDev();
		virtual ~CGGameDev();

		virtual void Initialize() override;
		virtual void Tick(float) override;

#if IS_EDITOR
#endif

	public:
	};
};
