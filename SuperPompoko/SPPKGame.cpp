#include "SPPKGame.h"
#include "Objects/CActor.h"
#include "Assets/CAsset.h"
#include "Assets/CAssetManager.h"

// �׽�Ʈ �ڵ�
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

		// �׽�Ʈ �ڵ�
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = SMGEGlobal::GetDirectoryCurrent();

		auto assetPath = CAssetManager::FindAssetFilePathByClassName(wtext("SMGE_Game::CMap"));
		CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(assetPath);
		currentMap_ = new CMap(*testMapTemplate->getContentClass());
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
		//CActor actor;
		//actor.setActorStaticTag("empty");

		//SGStringStreamOut strOut;
		//strOut << actor.getReflection();

		////const auto& aaa = actor.getConstReflection();	// const ��ü �׽�Ʈ

		//SGStringStreamIn strIn;
		//strIn.in_ = strOut.out_;
		//strIn >> actor.getReflection();
		//// }

		//// ���� ���ø� �ּ� �׽�Ʈ �ڵ� - CActor �� ��ũ�� �����ϱ� - ���� ���ø� �ּ��� �ȴ�
		//// {
		//actor.getWorldTransform()[3][3] = 333;
		//actor.setActorStaticTag("first asset test");

		//CAsset<CActor> actorAssetWriter(&actor);
		//CAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAssetWriter);

		//// �� ���ø� �ּ� �׽�Ʈ �ڵ� - CActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
		//CSharPtr<CAsset<CActor>> testActorTemplate = CAssetManager::LoadAsset<CActor>(assetRoot + wtext("testActorTemplate.asset"));
		//const auto& actorTemplate = *testActorTemplate->getContentClass();

		//CMap testMap;

		//CActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// ��ġ
		//CActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		//actorA.getWorldTransform()[0][1] = 345;	// ����
		//actorA.setActorStaticTag("AAA");

		//actorB.getWorldTransform()[1][3] = 987;
		//actorB.setActorStaticTag("BBB");

		//CAsset<CMap> mapAsset(&testMap);
		//CAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

		//// �𵨵����� �ּ� ���̺�, �ε��ϱ�
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

	//	// �� �ε��ϰ� ���͵� �ٽ� �����ϱ�
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
