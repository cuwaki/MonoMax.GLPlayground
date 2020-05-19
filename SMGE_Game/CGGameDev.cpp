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

		// 테스트 코드
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = wtext("e:/");
	}

	void CGGameDev::Tick(float dt)
	{
		Super::Tick(dt);

		CWString assetRoot = PathAssetRoot();


		//CGActor actor;
		//actor.setActorStaticTag("empty");

		//// 기본 리플렉션 테스트 코드
		//// {
		//SGStringStreamOut strOut;
		//strOut << actor.getReflection();

		////const auto& aaa = actor.getConstReflection();	// const 객체 테스트

		//SGStringStreamIn strIn;
		//strIn.in_ = strOut.out_;
		//strIn >> actor.getReflection();
		//// }

		//// 액터 템플릿 애셋 테스트 코드 - CGActor 를 디스크에 저장하기 - 액터 템플릿 애셋이 된다
		//// {
		//actor.getWorldTransform()[3][3] = 333;
		//actor.setActorStaticTag("first asset test");

		//CGAsset<CGActor> actorAssetWriter(&actor);
		//CGAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAssetWriter);



		
		// 맵 템플릿 애셋 테스트 코드 - CGActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
		//CSharPtr<CGAsset<CGActor>> testActorTemplate = CGAssetManager::LoadAsset<CGActor>(assetRoot + wtext("testActorTemplate.asset"));
		//const auto& actorTemplate = *testActorTemplate->getContentClass();

		//CGMap testMap;

		//CGActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// 배치
		//CGActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		//actorA.getWorldTransform()[0][1] = 345;	// 편집
		//actorA.setActorStaticTag("AAA");

		//actorB.getWorldTransform()[1][3] = 987;
		//actorB.setActorStaticTag("BBB");

		//CGAsset<CGMap> mapAsset(&testMap);
		//CGAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

		// 맵 로드하고 액터들 다시 복구하기
		CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetRoot + wtext("/map/testMapTemplate.asset"));
		CGMap loadedMap(*testMapTemplate->getContentClass());

		loadedMap.Activate();
		// }
	}
};
