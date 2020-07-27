#pragma once

#include "CGameBase.h"

//#define IS_GAME !(defined IS_EDITOR)

namespace SMGE
{
	class SPPKGame : public nsGE::CGameBase
	{
		using Super = nsGE::CGameBase;

	public:
		SPPKGame(CObject* outer);
		virtual ~SPPKGame();

	protected:
		virtual void Initialize() override;
		virtual void Tick(float) override;
		virtual void Render(float) override;

#if IS_EDITOR
#endif

	public:

	protected:
		// �׽�Ʈ �ڵ� - ��Ű���������δ� ���� ���δ� ���Ӽ��� ��ü�� �ʿ��ϰ����� �̴� ���Ŀ� ó������
		class CMap* currentMap_;
	};
};
