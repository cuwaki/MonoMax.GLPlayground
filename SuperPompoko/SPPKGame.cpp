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
	SPPKGame::SPPKGame(CObject* outer) : nsGE::CGameBase(outer)
	{
		Initialize();
	}

	SPPKGame::~SPPKGame()
	{
		currentMap_->FinishPlaying();
		delete currentMap_;

		delete gameSettings_;
		delete engine_;
	}

	void SPPKGame::Initialize()
	{
		engine_ = new nsGE::CEngineBase(this);
		gameSettings_ = new nsGE::SGEGameSettings();

		// �׽�Ʈ �ڵ�
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = Path::GetDirectoryCurrent();

		auto assetPath = CAssetManager::FindAssetFilePathByClassName(wtext("SMGE::CMap"));
		CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(assetPath);
		currentMap_ = new CMap(this, *testMapTemplate->getContentClass());
	}

#define EDITOR_WORKING

	void SPPKGame::Tick(float dt)
	{
		Super::Tick(dt);

#ifdef EDITOR_WORKING
		// �׽�Ʈ �ڵ�
		if (currentMap_->IsStarted() == false)
			currentMap_->StartToPlay();

		currentMap_->Tick(dt);
#else
		CWString assetRoot = PathAssetRoot();

		// �⺻ ���÷��� �׽�Ʈ �ڵ�
		// {
		CActor actor;
		actor.setActorStaticTag("empty");

		SGStringStreamOut strOut;
		strOut << actor.getReflection();

		//const auto& aaa = actor.getConstReflection();	// const ��ü �׽�Ʈ

		SGStringStreamIn strIn;
		strIn.in_ = strOut.out_;
		strIn >> actor.getReflection();
		// }

		// ���� ���ø� �ּ� �׽�Ʈ �ڵ� - CActor �� ��ũ�� �����ϱ� - ���� ���ø� �ּ��� �ȴ�
		// {
		actor.getWorldTransform()[3][3] = 333;
		actor.setActorStaticTag("first asset test");

		CAsset<CActor> actorAssetWriter(&actor);
		CAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAssetWriter);

		// �� ���ø� �ּ� �׽�Ʈ �ڵ� - CActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
		CSharPtr<CAsset<CActor>> testActorTemplate = CAssetManager::LoadAsset<CActor>(assetRoot + wtext("testActorTemplate.asset"));
		const auto& actorTemplate = *testActorTemplate->getContentClass();

		CMap testMap;

		CActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// ��ġ
		CActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		actorA.getWorldTransform() = glm::mat4(1);
		actorA.getWorldTransform() = glm::translate(actorA.getWorldTransform(), glm::vec3(5, 5, 0));
		actorA.setActorStaticTag("AAA");

		actorB.getWorldTransform() = glm::mat4(1);
		actorB.getWorldTransform() = glm::translate(actorB.getWorldTransform(), glm::vec3(-5, -5, 0));
		actorB.setActorStaticTag("BBB");

		CAsset<CMap> mapAsset(&testMap);
		CAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

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
#endif
	}

	void SPPKGame::Render(float dt)
	{
		Super::Render(dt);

		currentMap_->Render(dt);
	}
};
