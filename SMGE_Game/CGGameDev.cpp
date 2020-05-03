#include "CGGameDev.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// �׽�Ʈ �ڵ�
#include "../SMGE/CGEEngineBase.h"

namespace MonoMaxGraphics
{
	CGGameDev::CGGameDev()
	{
		engine_ = new CGEEngineBase();
	}

	CGGameDev::~CGGameDev()
	{
		delete engine_;
	}

	void CGGameDev::Tick(float dt)
	{
		Super::Tick(dt);

		CGActor actor;

		SGStringStreamOut strOut;
		strOut << actor.getReflection();

		//const auto& aaa = actor.getConstReflection();	// const ��ü �׽�Ʈ

		SGStringStreamIn strIn;
		strIn.in_ = strOut.out_;
		strIn >> actor.getReflection();

		// CGActor �� ��ũ�� �����ϱ� - ���� ���ø� �ּ��� �ȴ�
		CGAsset<CGActor> actorAsset(&actor);
		CGAssetManager::SaveAsset(wtext("c:\\testasset.asset"), actorAsset);

		CSharPtr<CGAsset<CGActor>> loaded = CGAssetManager::LoadAsset<CGActor>(wtext("c:\\testasset.asset"));

		// CGActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
	}
};
