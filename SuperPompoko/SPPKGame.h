#pragma once

#include "CGameBase.h"

//#define IS_GAME !(defined IS_EDITOR)

namespace SMGE
{
	class SPPKGame : public CGameBase
	{
		using Super = CGameBase;

	public:
		SPPKGame(CObject* outer);
		virtual ~SPPKGame();

	protected:
		virtual void Initialize() override;
		virtual void Tick(float) override;

#if IS_EDITOR
#endif

	public:
	};
};
