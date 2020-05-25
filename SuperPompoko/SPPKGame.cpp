#include "SPPKGame.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// �׽�Ʈ �ڵ�
#include "../SMGE/CGEEngineBase.h"
#include "../SMGE_Game/Objects/CGMap.h"
#include "../SMGE_Game/Assets/CGModelData.h"

namespace SMGE
{
	SPPKGame::SPPKGame() : nsGE::CGEGameBase()
	{
		Initialize();
	}

	SPPKGame::~SPPKGame()
	{
		delete currentMap_;
	}

	void SPPKGame::Initialize()
	{
		engine_ = new nsGE::CGEEngineBase();
		gameSettings_ = new nsGE::SGEGameSettings();

		// �׽�Ʈ �ڵ�
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = SMGEGlobal::GetDirectoryCurrent();

		auto assetPath = CGAssetManager::FindAssetFilePathByClassName(wtext("SMGE_Game::CGMap"));
		CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetPath);
		currentMap_ = new CGMap(*testMapTemplate->getContentClass());
	}

	void SPPKGame::Tick(float dt)
	{
		Super::Tick(dt);

		// �׽�Ʈ �ڵ�
		if(currentMap_->IsStarted() == false)
			currentMap_->StartToPlay();

		currentMap_->Tick(dt);

		//CWString assetRoot = PathAssetRoot();

		//// �⺻ ���÷��� �׽�Ʈ �ڵ�
		//// {
		//CGActor actor;
		//actor.setActorStaticTag("empty");

		//SGStringStreamOut strOut;
		//strOut << actor.getReflection();

		////const auto& aaa = actor.getConstReflection();	// const ��ü �׽�Ʈ

		//SGStringStreamIn strIn;
		//strIn.in_ = strOut.out_;
		//strIn >> actor.getReflection();
		//// }

		//// ���� ���ø� �ּ� �׽�Ʈ �ڵ� - CGActor �� ��ũ�� �����ϱ� - ���� ���ø� �ּ��� �ȴ�
		//// {
		//actor.getWorldTransform()[3][3] = 333;
		//actor.setActorStaticTag("first asset test");

		//CGAsset<CGActor> actorAssetWriter(&actor);
		//CGAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAssetWriter);

		//// �� ���ø� �ּ� �׽�Ʈ �ڵ� - CGActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
		//CSharPtr<CGAsset<CGActor>> testActorTemplate = CGAssetManager::LoadAsset<CGActor>(assetRoot + wtext("testActorTemplate.asset"));
		//const auto& actorTemplate = *testActorTemplate->getContentClass();

		//CGMap testMap;

		//CGActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// ��ġ
		//CGActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		//actorA.getWorldTransform()[0][1] = 345;	// ����
		//actorA.setActorStaticTag("AAA");

		//actorB.getWorldTransform()[1][3] = 987;
		//actorB.setActorStaticTag("BBB");

		//CGAsset<CGMap> mapAsset(&testMap);
		//CGAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

		//// �𵨵����� �ּ� ���̺�, �ε��ϱ�
		//CGModelData modelData;
		//CGAsset<CGModelData> modelDataAsset(&modelData);

		//modelData.vertShaderPath_ = wtext("simple_color_vs.glsl");
		//modelData.fragShaderPath_ = wtext("simple_color_fs.glsl");
		//modelData.vertices_ = std::initializer_list<float>
		//{
		//	0.0f, 0.5f, 0.0f,
		//	-0.5f, -0.5f, 0.0f,
		//	0.5f, -0.5f, 0.0f,
		//};
		//modelData.vertexAttribNumber_ = 3;
		//CGAssetManager::SaveAsset(assetRoot + wtext("/mesh/testTriangle.asset"), modelDataAsset);
		//CSharPtr<CGAsset<CGModelData>> testTriangle = CGAssetManager::LoadAsset<CGModelData>(assetRoot + wtext("/mesh/testTriangle.asset"));

	//	// �� �ε��ϰ� ���͵� �ٽ� �����ϱ�
	//	CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetRoot + wtext("/map/testMapTemplate.asset"));
	//	CGMap loadedMap(*testMapTemplate->getContentClass());
	//	loadedMap.Activate();
	//	// }
	}

	void SPPKGame::Render(float dt)
	{
		Super::Render(dt);

		currentMap_->Render(dt);
	}
};
