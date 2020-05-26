#pragma once

#include "CGameBase.h"

#define IS_EDITOR 1
#define IS_GAME !(defined IS_EDITOR)

namespace SMGE
{
	class SPPKGame : public nsGE::CGameBase
	{
		using Super = nsGE::CGameBase;

	public:
		SPPKGame();
		virtual ~SPPKGame();

	protected:
		virtual void Initialize() override;
		virtual void Tick(float) override;
		virtual void Render(float) override;

#if IS_EDITOR
#endif

	public:

	protected:
		// 테스트 코드 - 아키텍터적으로는 맵을 감싸는 게임서버 객체가 필요하겠지만 이는 차후에 처리하자
		class CMap* currentMap_;
	};
};
