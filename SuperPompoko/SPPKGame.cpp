#include "SPPKGame.h"
#include "Objects/CActor.h"
#include "Assets/CAsset.h"
#include "Assets/CAssetManager.h"

// 테스트 코드
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Assets/CResourceModel.h"
#include "Components/CPointComponent.h"

// 테스트 코드 ㅡ 쿼트트리
#include "CQuadTree.h"
#include <set>

class TCActor
{
public:
	TCActor(int index, int x, int y, int z) : index(index), x(x), y(y), z(z)
	{
	}
	int index;
	int x, y, z;
	int xS = 100, yS = 100, zS = 100;
};


namespace SMGE
{
	namespace Globals
	{
		CWString GetGameProjectName()
		{
			return wtext("SPPK");
		}
		CWString GetEngineAssetPath(const CWString& assetFilePath)
		{
#if DEBUG || _DEBUG
			return wtext("./RunningResources/") + assetFilePath;
#else
#endif
		}
		CWString GetGameAssetRoot()
		{
#if DEBUG || _DEBUG
			// 개발중에는 실행 dir 을 프로젝트 루트로 맞춰라 - .sln 이 있는 곳 말이다!
			return wtext("./RunningResources/") + GetGameProjectName() + wtext("/assets/");
#else
#endif
		}

#if IS_EDITOR
		CWString GetGameProjectRoot()
		{
			auto ret = Path::GetNormalizedPath(Path::GetDirectoryCurrent());
			CuwakiDevUtils::ReplaceInline(ret, wtext("/Debug"), wtext(""));
			CuwakiDevUtils::ReplaceInline(ret, wtext("/Release"), wtext(""));
			CuwakiDevUtils::ReplaceInline(ret, wtext("/Shipping"), wtext(""));
			return ret;
		}
#else
		CWString GetGameProjectRoot()
		{
			return Path::GetNormalizedPath(Path::GetDirectoryCurrent());
		}
#endif
	}

	SPPKGame::SPPKGame(CObject* outer) : nsGE::CGameBase(outer)
	{
		Initialize();

		Globals::GCurrentGame = this;

		// 테스트 코드 ㅡ 쿼드트리
		{
			auto maxX = 10000, maxY = 10000, maxZ = 10000;
			auto screenW = 1024, screenH = 768;

			std::vector<TCActor> tcactors;
			tcactors.reserve(100);
			for (int k = 0; k < 100; ++k)
			{
				tcactors.push_back(
					{
						k,
						std::rand() % maxX - (maxX / 2),
						std::rand() % maxY - (maxY / 2),
						std::rand() % maxZ - (maxZ / 2),
					});
			}

			using TTCActorOT = COcTree<std::set<TCActor*>, int32_t>;
			using OTNode = TTCActorOT::TNode;
			using OTNodeData = TTCActorOT::TNode::TValue;

			TTCActorOT octreeActor("xyz", maxX, maxY, maxZ, screenW);

			// 최초 TCActor 들을 배치한다
			for (auto& tca : tcactors)
			{
				OTNode* xyNode = octreeActor.QueryNodeByXY(tca.x, tca.y);
				OTNode* zxNode = octreeActor.QueryNodeByZX(tca.z, tca.x);

				if (xyNode && zxNode)
				{
					xyNode->GetContainer().insert(&tca);
					zxNode->GetContainer().insert(&tca);
				}
			}

			// 업데이트한다
			for (auto& tca : tcactors)
			{
				OTNode* OxyNode = octreeActor.QueryNodeByXY(tca.x, tca.y);
				OTNode* OzxNode = octreeActor.QueryNodeByZX(tca.z, tca.x);

				tca.TCActor::TCActor({
						tca.index,
						std::rand() % maxX - (maxX / 2),
						std::rand() % maxY - (maxY / 2),
						std::rand() % maxZ - (maxZ / 2),
					});	// 위치 변경

				OTNode* NxyNode = octreeActor.QueryNodeByXY(tca.x, tca.y);
				OTNode* NzxNode = octreeActor.QueryNodeByZX(tca.z, tca.x);

				if (OxyNode != NxyNode)
				{
					NxyNode->GetContainer().insert(&tca);
					OxyNode->GetContainer().erase(&tca);
				}
				if (OzxNode != NzxNode)
				{
					NzxNode->GetContainer().insert(&tca);
					OzxNode->GetContainer().erase(&tca);
				}
			}

			// 찾기
			for (int tcaI = 0; tcaI < tcactors.size(); ++tcaI)
			{
				auto foundVec = octreeActor.QueryValuesByPoint(tcactors[tcaI].x, tcactors[tcaI].y, tcactors[tcaI].z);
				if (foundVec.size() > 0)
				{	// 점쿼리는 이상 없이 잘 되고
					volatile int xxx = 0;
				}

				// 여기 - 아 이상하다 균등이든 비균등이든 4번을 큐브로 컬링하면 안나온다
				glm::vec3 _4Loc(tcactors[tcaI].x, tcactors[tcaI].y, tcactors[tcaI].z);
				auto _4LB = _4Loc - tcactors[tcaI].xS / 2.f;
				auto _4RT = _4Loc + tcactors[tcaI].xS / 2.f;
				//glm::vec3 _4Loc(-5000.f + 4.f, -5000.f + 4.f, -5000.f + 4.f), _4Size(2.f, 2.f, 2.f);
				//auto _4LB = _4Loc - _4Size / 2.f;
				//auto _4RT = _4Loc + _4Size / 2.f;
				foundVec = octreeActor.QueryValuesByCube(_4LB.x, _4LB.y, _4LB.z, _4RT.x, _4RT.y, _4RT.z);
				if (foundVec.size() > 0)
				{
					volatile int xxx = 0;
				}
			}

			// 삭제한다
			for (auto& tca : tcactors)
			{
				OTNode* OxyNode = octreeActor.QueryNodeByXY(tca.x, tca.y);
				OTNode* OzxNode = octreeActor.QueryNodeByZX(tca.z, tca.x);
				OxyNode->GetContainer().erase(&tca);
				OzxNode->GetContainer().erase(&tca);
			}

			/*  쿼드 트리 테스트
			using TTCActorQT = CQuadTree<std::set<TCActor*>>;
			TTCActorQT qtActorXY("xy", 10000, 10000, 1024, 768);
			TTCActorQT qtActorZX("zx", 10000, 10000, 1024, 768);

			using QTNode = TTCActorQT::TNode;
			using QTNodeData = TTCActorQT::TNode::TValue;

			// 최초 TCActor 들을 qt에 배치한다
			for (auto& tca : tcactors)
			{
				QTNode* node = qtActorXY.QueryNodeByPoint(tca.x, tca.y);
				node->GetContainer().insert(&tca);

				node = qtActorZX.QueryNodeByPoint(tca.z, tca.x);
				node->GetContainer().insert(&tca);

				volatile int xxx = 0;
			}

			// 업데이트한다
			for (auto& tca : tcactors)
			{
				QTNode* oldNodeXY = qtActorXY.QueryNodeByPoint(tca.x, tca.y);
				QTNode* oldNodeZX = qtActorZX.QueryNodeByPoint(tca.z, tca.x);

				tca.TCActor::TCActor({
						tca.index,
						std::rand() % maxX - (maxX / 2),
						std::rand() % maxY - (maxY / 2),
						std::rand() % maxZ - (maxZ / 2),
					});	// 위치 변경

				QTNode* newNodeXY = qtActorXY.QueryNodeByPoint(tca.x, tca.y);
				QTNode* newNodeZX = qtActorZX.QueryNodeByPoint(tca.z, tca.x);

				if (oldNodeXY != newNodeXY)
				{
					newNodeXY->GetContainer().insert(&tca);
					oldNodeXY->GetContainer().erase(&tca);
				}
				if (oldNodeZX != newNodeZX)
				{
					newNodeZX->GetContainer().insert(&tca);
					oldNodeZX->GetContainer().erase(&tca);
				}
			}

			// 옥트리로써 찾아본다
			glm::vec3 findingLoc(tcactors[4].x, tcactors[4].y, tcactors[4].z);
			QTNodeData& dataXY = qtActorXY.QueryNodeByPoint(findingLoc.x, findingLoc.y)->GetContainer();
			QTNodeData& dataZX = qtActorZX.QueryNodeByPoint(findingLoc.z, findingLoc.x)->GetContainer();

			std::set<QTNodeData::value_type> intersect;
			std::set_intersection(dataXY.begin(), dataXY.end(), dataZX.begin(), dataZX.end(), std::inserter(intersect, intersect.begin()));

			// 삭제한다
			for (auto& tca : tcactors)
			{
				QTNode* oldNode = qtActorXY.QueryNodeByPoint(tca.x, tca.y);
				oldNode->GetContainer().erase(&tca);
			}
			*/
		}
	}

	SPPKGame::~SPPKGame()
	{
		currentMap_->FinishPlaying();
		delete currentMap_;

		delete gameSettings_;
		delete engine_;

		Globals::GCurrentGame = nullptr;
		Globals::GCurrentMap = nullptr;
	}

#define EDITOR_WORKING

	void SPPKGame::Initialize()
	{
		engine_ = new nsGE::CEngineBase(this);
		gameSettings_ = new nsGE::SGEGameSettings();

		// 테스트 코드
		gameSettings_->gameProjectName_ = Globals::GetGameProjectName();
		gameSettings_->gameProjectRootPath_ = Globals::GetGameProjectRoot();

#ifdef EDITOR_WORKING
		// 테스트 코드
		CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(Globals::GetGameAssetPath(wtext("/map/testMap.asset")));
		currentMap_ = new CMap(this, *testMapTemplate->getContentClass());

		Globals::GCurrentMap = currentMap_;

		auto EditorProcessUserInput = [this](const nsGE::CUserInput& userInput)
		{
			if (userInput.IsJustPressed(nsGE::CUserInput::LBUTTON))
			{
				// 테스트 코드 - 동적 액터 스폰 샘플 코드 {
				auto mouseScreenPos = userInput.GetMousePosition();

				glm::vec3 ray_origin;
				glm::vec3 ray_direction;
				engine_->GetRenderingEngine()->ScreenPosToWorld(mouseScreenPos, ray_origin, ray_direction);

				CRayCollideActor* rayActor = &currentMap_->SpawnDefaultActor<CRayCollideActor>(true,
					ECheckCollideRule::NEAREST, false,
					[this](class CActor* SRC, class CActor* TAR, const class CBoundComponent* SRC_BOUND, const class CBoundComponent* TAR_BOUND, const glm::vec3& COLL_POS)
					{
						CPointActor* pointActor = &currentMap_->SpawnDefaultActor<CPointActor>(true);
						currentMap_->FinishSpawnActor(*pointActor);

						pointActor->getTransform().Translate(COLL_POS);
						pointActor->SetLifeTick(100);
					},
					engine_->GetRenderingEngine()->GetCamera()->GetZFar(), ray_direction);
				currentMap_->FinishSpawnActor(*rayActor);

				// 생각 - 이걸 비긴 플레이로 넣고 싶은데 / 애프터 비긴 플레이 말고 그냥 비긴 플레이에 넣을 수는 없을까?
				rayActor->getTransform().Translate(ray_origin);
				auto targets = rayActor->QueryCollideCheckTargets();
				rayActor->ProcessCollide(targets);
				rayActor->SetLifeTick(2);
				// }
			}

			return false;
		};
		engine_->AddProcessUserInputs(EditorProcessUserInput);
#endif
	}

	void SPPKGame::Tick(float dt)
	{
		Super::Tick(dt);

#ifdef EDITOR_WORKING
		// 테스트 코드
		if (currentMap_->IsStarted() == false)
		{
			currentMap_->StartToPlay();
		}

		currentMap_->Tick(dt);
		currentMap_->ProcessPendingKills();
#else
		CWString assetRoot = PathAssetRoot();

		//// 기본 리플렉션 테스트 코드
		//CActor actor(nullptr);
		//actor.setActorStaticTag("empty");
		//SGStringStreamOut strOut;
		//strOut << actor.getReflection();

		////const auto& aaa = actor.getConstReflection();	// const 객체 테스트
		//SGStringStreamIn strIn;
		//strIn.in_ = strOut.out_;
		//strIn >> actor.getReflection();
		//// }

		// 액터 템플릿 애셋 테스트 코드 - CActor 를 디스크에 저장하기 - 액터 템플릿 애셋이 된다
		// {
		// 여기부터 
			auto actorAssetPath = assetRoot + wtext("/actor/forceSaveActor.asset");
			CActor savingActor(nullptr);
			savingActor.setActorStaticTag("force save");
			CAsset<CActor> actorAssetWriter(&savingActor);
			CAssetManager::SaveAsset(actorAssetPath, actorAssetWriter);

			// 액터 강제 로드 테스트		
			CSharPtr<CAsset<CActor>> loadingActorTemplate = CAssetManager::LoadAsset<CActor>(actorAssetPath);
			auto loadedActor = new CActor(this, *loadingActorTemplate->getContentClass());
		// 여기까지 액터 저장 및 로드



		//// 맵 템플릿 애셋 테스트 코드 - CActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
		//CSharPtr<CAsset<CActor>> testActorTemplate = CAssetManager::LoadAsset<CActor>(assetRoot + wtext("testActorTemplate.asset"));
		//const auto& actorTemplate = *testActorTemplate->getContentClass();

		//CMap testMap;

		//CActor& actorA = testMap.SpawnDefaultActor(actorTemplate, true);	// 배치
		//CActor& actorB = testMap.SpawnDefaultActor(actorTemplate, true);

		//actorA.getWorldTransform() = glm::mat4(1);
		//actorA.getWorldTransform() = glm::translate(actorA.getWorldTransform(), glm::vec3(5, 5, 0));
		//actorA.setActorStaticTag("AAA");

		//actorB.getWorldTransform() = glm::mat4(1);
		//actorB.getWorldTransform() = glm::translate(actorB.getWorldTransform(), glm::vec3(-5, -5, 0));
		//actorB.setActorStaticTag("BBB");

		//CAsset<CMap> mapAsset(&testMap);
		//CAssetManager::SaveAsset(assetRoot + wtext("/map/testMapTemplate.asset"), mapAsset);

		/* 애셋모델 저장하기 - 모델데이터 애셋 세이브, 로드하기
		CResourceModel modelData(nullptr);
		CAsset<CResourceModel> modelDataAsset(&modelData);

		modelData.vertShaderPath_ = wtext("");
		modelData.fragShaderPath_ = wtext("suzanne.frag");
		modelData.objFilePath_ = wtext("suzanne.obj");
		modelData.textureFilePath_ = wtext("");

		CVector<glm::vec3> planeVertices
		{
			{-1.0f, 0, -1.0f},
			{ -1.0f, 0, 1.0f},
			{ 1.0f, 0,  -1.0f},

			{ -1.0f, 0, 1.0f},
			{ 1.0f, 0, 1.0f},
			{ 1.0f, 0,  -1.0f},
		};
		CVector<glm::vec2> planeUvs{ {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
		CVector<glm::vec3> planeNormals{ {0,1,0}, {0,1,0},{0,1,0},{0,1,0},{0,1,0},{0,1,0} };
		CVector<glm::vec3> planeVertexColors{ {0,0.5,0}, {0,0.5,0},{0,0.5,0},{0,0.5,0},{0,0.5,0},{0,0.5,0} };


		CVector<glm::vec3> cubeVertices
		{
			{ -1.0f,-1.0f,-1.0f,},{ -1.0f,-1.0f, 1.0f,},{ -1.0f, 1.0f, 1.0f,},
			{  1.0f, 1.0f,-1.0f,},{ -1.0f,-1.0f,-1.0f,},{ -1.0f, 1.0f,-1.0f,},
			{  1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f,-1.0f,},{  1.0f,-1.0f,-1.0f,},
			{  1.0f, 1.0f,-1.0f,},{  1.0f,-1.0f,-1.0f,},{ -1.0f,-1.0f,-1.0f,},
			{ -1.0f,-1.0f,-1.0f,},{ -1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f,-1.0f,},
			{  1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f, 1.0f,},{ -1.0f,-1.0f,-1.0f,},
			{ -1.0f, 1.0f, 1.0f,},{ -1.0f,-1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f,},
			{  1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f,-1.0f,},{  1.0f, 1.0f,-1.0f,},
			{  1.0f,-1.0f,-1.0f,},{  1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f,},
			{  1.0f, 1.0f, 1.0f,},{  1.0f, 1.0f,-1.0f,},{ -1.0f, 1.0f,-1.0f,},
			{  1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f,-1.0f,},{ -1.0f, 1.0f, 1.0f,},
			{  1.0f, 1.0f, 1.0f,},{ -1.0f, 1.0f, 1.0f,},{  1.0f,-1.0f, 1.0f	},
		};
		CVector<glm::vec2> cubeUvs;
		cubeUvs.resize(cubeVertices.size());	// uv are all 000

		CVector<glm::vec3> cubeNormals;	// 각 삼각형에 대하여 정점 normal, vertColor 만들어주기
		CVector<glm::vec3> cubeVertexColors;
		cubeNormals.reserve(cubeVertices.size());
		cubeVertexColors.reserve(cubeVertices.size());

		for (size_t ii = 0; ii < cubeVertices.size(); ii += 3)
		{
			glm::vec3 _0 = cubeVertices[ii + 0], _1 = cubeVertices[ii + 1], _2 = cubeVertices[ii + 2];
			glm::vec3 _0_to_1 = _1 - _0, _0_to_2 = _2 - _0;
			glm::vec3 face_normal = glm::normalize(glm::cross(_0_to_1, _0_to_2));

			cubeNormals.push_back(face_normal);
			cubeVertexColors.push_back({ 0.5,0,0 });

			cubeNormals.push_back(face_normal);
			cubeVertexColors.push_back({ 0,0.5,0 });

			cubeNormals.push_back(face_normal);
			cubeVertexColors.push_back({ 0,0,0.5 });
		}

		modelData.vertices_ = cubeVertices;
		modelData.uvs_ = cubeUvs;
		modelData.normals_ = cubeNormals;
		modelData.vertexColors_ = cubeVertexColors;

		CAssetManager::SaveAsset(assetRoot + wtext("/models/cube/cube.asset"), modelDataAsset);
		CSharPtr<CAsset<CResourceModel>> amA = CAssetManager::LoadAsset<CResourceModel>(assetRoot + wtext("/models/cube/cube.asset"));
		애셋모델 저장하기 */

	//	// 맵 로드하고 액터들 다시 복구하기
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
