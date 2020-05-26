#include "SPPKGame.h"
#include "Objects/CActor.h"
#include "Assets/CAsset.h"
#include "Assets/CAssetManager.h"

// 테스트 코드
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Assets/CModelData.h"

namespace SMGE
{
	SPPKGame::SPPKGame() : nsGE::CGameBase()
	{
		Initialize();
	}

	SPPKGame::~SPPKGame()
	{
		delete currentMap_;
	}

	void SPPKGame::Initialize()
	{
		engine_ = new nsGE::CEngineBase();
		gameSettings_ = new nsGE::SGEGameSettings();

		// 테스트 코드
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = SMGEGlobal::GetDirectoryCurrent();

		auto assetPath = CAssetManager::FindAssetFilePathByClassName(wtext("SMGE_Game::CMap"));
		CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(assetPath);
		currentMap_ = new CMap(*testMapTemplate->getContentClass());
	}

	void SPPKGame::Tick(float dt)
	{
		Super::Tick(dt);

		// 테스트 코드
		if(currentMap_->IsStarted() == false)
			currentMap_->StartToPlay();

		currentMap_->Tick(dt);

		//CWString assetRoot = PathAssetRoot();

		//// 기본 리플렉션 테스트 코드
		//// {
		//CActor actor;
		//actor.setActorStaticTag("empty");

		//SGStringStreamOut strOut;
		//strOut << actor.getReflection();

		////const auto& aaa = actor.getConstReflection();	// const 객체 테스트

		//SGStringStreamIn strIn;
		//strIn.in_ = strOut.out_;
		//strIn >> actor.getReflection();
		//// }

		//// 액터 템플릿 애셋 테스트 코드 - CActor 를 디스크에 저장하기 - 액터 템플릿 애셋이 된다
		//// {
		//actor.getWorldTransform()[3][3] = 333;
		//actor.setActorStaticTag("first asset test");

		//CAsset<CActor> actorAssetWriter(&actor);
		//CAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAssetWriter);

		//// 맵 템플릿 애셋 테스트 코드 - CActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
		//CSharPtr<CAsset<CActor>> testActorTemplate = CAssetManager::LoadAsset<CActor>(assetRoot + wtext("testActorTemplate.asset"));
		//const auto& actorTemplate = *testActorTemplate->getContentClass();

		//CMap testMap;

		//CActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// 배치
		//CActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		//actorA.getWorldTransform()[0][1] = 345;	// 편집
		//actorA.setActorStaticTag("AAA");

		//actorB.getWorldTransform()[1][3] = 987;
		//actorB.setActorStaticTag("BBB");

		//CAsset<CMap> mapAsset(&testMap);
		//CAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

		//// 모델데이터 애셋 세이브, 로드하기
		//CModelData modelData;
		//CAsset<CModelData> modelDataAsset(&modelData);

		//modelData.vertShaderPath_ = wtext("simple_color_vs.glsl");
		//modelData.fragShaderPath_ = wtext("simple_color_fs.glsl");
		//modelData.vertices_ = std::initializer_list<float>
		//{
		//	0.0f, 0.5f, 0.0f,
		//	-0.5f, -0.5f, 0.0f,
		//	0.5f, -0.5f, 0.0f,
		//};
		//modelData.vertexAttribNumber_ = 3;
		//CAssetManager::SaveAsset(assetRoot + wtext("/mesh/testTriangle.asset"), modelDataAsset);
		//CSharPtr<CAsset<CModelData>> testTriangle = CAssetManager::LoadAsset<CModelData>(assetRoot + wtext("/mesh/testTriangle.asset"));

	//	// 맵 로드하고 액터들 다시 복구하기
	//	CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(assetRoot + wtext("/map/testMapTemplate.asset"));
	//	CMap loadedMap(*testMapTemplate->getContentClass());
	//	loadedMap.Activate();
	//	// }
	}

	void SPPKGame::Render(float dt)
	{
		Super::Render(dt);

		currentMap_->Render(dt);
	}
};
