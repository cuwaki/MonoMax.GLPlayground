#pragma once

#include "../SMGE/CGEGameBase.h"

#define IS_EDITOR 1
#define IS_GAME !(defined IS_EDITOR)

namespace SMGE
{
	class SPPKGame : public nsGE::CGEGameBase
	{
		using Super = nsGE::CGEGameBase;

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
		// �׽�Ʈ �ڵ� - ��Ű���������δ� ���� ���δ� ���Ӽ��� ��ü�� �ʿ��ϰ����� �̴� ���Ŀ� ó������
		class CGMap* currentMap_;
	};
};
