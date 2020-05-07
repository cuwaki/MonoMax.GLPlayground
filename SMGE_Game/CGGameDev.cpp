#include "CGGameDev.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// 테스트 코드
#include "../SMGE/CGEEngineBase.h"
#include "../SMGE_Game/Objects/CGMap.h"

namespace MonoMaxGraphics
{
	CGGameDev::CGGameDev() : CGEGameBase()
	{
		Initialize();
	}

	CGGameDev::~CGGameDev()
	{
	}

	void CGGameDev::Initialize()
	{
		engine_ = new CGEEngineBase();
		gameSettings_ = new SGEGameSettings();

		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = wtext("c:/");
	}

	void CGGameDev::Tick(float dt)
	{
		Super::Tick(dt);

		CGActor actor;
		actor.CGCtor();

		// 기본 리플렉션 테스트 코드
		// {
		SGStringStreamOut strOut;
		strOut << actor.getReflection();

		//const auto& aaa = actor.getConstReflection();	// const 객체 테스트

		SGStringStreamIn strIn;
		strIn.in_ = strOut.out_;
		strIn >> actor.getReflection();
		// }

		CWString assetRoot = PathAssetRoot();

		// 액터 템플릿 애셋 테스트 코드 - CGActor 를 디스크에 저장하기 - 액터 템플릿 애셋이 된다
		// {
		CGAsset<CGActor> actorAsset(&actor);
		CGAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAsset);
		CSharPtr<CGAsset<CGActor>> testActorTemplate = CGAssetManager::LoadAsset<CGActor>(assetRoot + wtext("testActorTemplate.asset"));
		// }

		// 맵 템플릿 애셋 테스트 코드 - CGActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
		// {
		const auto& actorTemplate = *testActorTemplate->getContentClass();

		CGMap tempMap;
		tempMap.CGCtor();

		CGActor& actorA = tempMap.ArrangeActor(actorTemplate);	// 배치
		CGActor& actorB = tempMap.ArrangeActor(actorTemplate);

		actorA.getWorldTransform()[0][1] = 345;	// 편집
		actorB.getWorldTransform()[1][3] = 987;

		CGAsset<CGMap> mapAsset(&tempMap);
		CGAssetManager::SaveAsset(assetRoot + wtext("/map/testMap.asset"), mapAsset);	// 여기 할 차례

		// 맵 로드하고 액터들 다시 복구하기
		CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetRoot + wtext("/map/testMap.asset"));
		CGMap* loadedMap = testMapTemplate->getContentClass();
		loadedMap->Activate();
		// }
	}
};
