#pragma once

#include "../SMGE/CGEGameBase.h"

#define IS_EDITOR 1
#define IS_GAME !(defined IS_EDITOR)

namespace MonoMaxGraphics
{
	class SPPKGame : public CGEGameBase
	{
		using Super = CGEGameBase;

	public:
		SPPKGame();
		virtual ~SPPKGame();

		virtual void Initialize() override;
		virtual void Tick(float) override;

#if IS_EDITOR
#endif

	public:
	};
};
