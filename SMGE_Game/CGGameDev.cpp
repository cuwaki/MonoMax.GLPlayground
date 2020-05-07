#include "CGGameDev.h"
#include "Objects/CGActor.h"
#include "Assets/CGAsset.h"
#include "Assets/CGAssetManager.h"

// �׽�Ʈ �ڵ�
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

		// �⺻ ���÷��� �׽�Ʈ �ڵ�
		// {
		SGStringStreamOut strOut;
		strOut << actor.getReflection();

		//const auto& aaa = actor.getConstReflection();	// const ��ü �׽�Ʈ

		SGStringStreamIn strIn;
		strIn.in_ = strOut.out_;
		strIn >> actor.getReflection();
		// }

		CWString assetRoot = PathAssetRoot();

		// ���� ���ø� �ּ� �׽�Ʈ �ڵ� - CGActor �� ��ũ�� �����ϱ� - ���� ���ø� �ּ��� �ȴ�
		// {
		CGAsset<CGActor> actorAsset(&actor);
		CGAssetManager::SaveAsset(assetRoot + wtext("testActorTemplate.asset"), actorAsset);
		CSharPtr<CGAsset<CGActor>> testActorTemplate = CGAssetManager::LoadAsset<CGActor>(assetRoot + wtext("testActorTemplate.asset"));
		// }

		// �� ���ø� �ּ� �׽�Ʈ �ڵ� - CGActor �� �ʿ� �����ϱ� - �ʿ� ��ġ�� �� ������ ������, ���� �ε�� �� ���Ͱ� ��ġ�� �� �� ������ ������ �ȴ�
		// {
		const auto& actorTemplate = *testActorTemplate->getContentClass();

		CGMap tempMap;
		tempMap.CGCtor();

		CGActor& actorA = tempMap.ArrangeActor(actorTemplate);	// ��ġ
		CGActor& actorB = tempMap.ArrangeActor(actorTemplate);

		actorA.getWorldTransform()[0][1] = 345;	// ����
		actorB.getWorldTransform()[1][3] = 987;

		CGAsset<CGMap> mapAsset(&tempMap);
		CGAssetManager::SaveAsset(assetRoot + wtext("/map/testMap.asset"), mapAsset);	// ���� �� ����

		// �� �ε��ϰ� ���͵� �ٽ� �����ϱ�
		CSharPtr<CGAsset<CGMap>> testMapTemplate = CGAssetManager::LoadAsset<CGMap>(assetRoot + wtext("/map/testMap.asset"));
		CGMap* loadedMap = testMapTemplate->getContentClass();
		loadedMap->Activate();
		// }
	}
};
