#include "CGGameDev.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// 테스트 코드
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

		//const auto& aaa = actor.getConstReflection();	// const 객체 테스트

		SGStringStreamIn strIn;
		strIn.in_ = strOut.out_;
		strIn >> actor.getReflection();

		// CGActor 를 디스크에 저장하기 - 액터 템플릿 애셋이 된다
		CGAsset<CGActor> actorAsset(&actor);
		CGAssetManager::SaveAsset(wtext("c:\\testasset.asset"), actorAsset);

		CSharPtr<CGAsset<CGActor>> loaded = CGAssetManager::LoadAsset<CGActor>(wtext("c:\\testasset.asset"));

		// CGActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
	}
};
