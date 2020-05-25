#include "CGGameDev.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// �׽�Ʈ �ڵ�
#include "../SMGE/CGEEngineBase.h"
#include "../SMGE_Game/Objects/CGMap.h"

namespace SMGE
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

		// �׽�Ʈ �ڵ�
		gameSettings_->gameProjectName_ = wtext("dev_project");
		gameSettings_->gameProjectRootPath_ = wtext("e:/");
	}

	void CGGameDev::Tick(float dt)
	{
		Super::Tick(dt);

		CWString assetRoot = PathAssetRoot();

		//CGActor actor;
		//actor.setActorStaticTag("empty");

		//// �⺻ ���÷��� �׽�Ʈ �ڵ�
		//// {
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

		// �� ���ø� �ּ� �׽�Ʈ �ڵ� - CGActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
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

		// �� �ε��ϰ� ���͵� �ٽ� �����ϱ�
		CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetRoot + wtext("/map/testMapTemplate.asset"));
		CGMap loadedMap(*testMapTemplate->getContentClass());

		loadedMap.Activate();
		// }
	}
};
