#include "SPPKGame.h"
#include "Objects/CActor.h"
#include "Assets/CAsset.h"
#include "Assets/CAssetManager.h"

// 테스트 코드
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Assets/CResourceModel.h"
#include "Components/CPointComponent.h"
#include "Components/CRayComponent.h"
#include "Objects/CCollideActor.h"
#include "Assets/CAssetManager.h"
#include "Assets/CAsset.h"
#include "Objects/CStaticMeshActor.h"
#include "CBoundCheck.h"

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
		// 테스트 코드
		//{
		glm::vec3 out;
		SSegmentBound outSeg;
		bool rr;

		//세그먼트와 플레인
		//SPlaneBound plane({ 0,1,0 }, { 0,0,0 });
		//SSegmentBound layonUpPlane({ 1,1,1 }, { -1,-1,-1 });
		//SSegmentBound layonBelowPlane({ -1,-1,-1 }, { 1,1,1 });

		//layonUpPlane.check(plane, out);
		//layonBelowPlane.check(plane, out);

		// 세그먼트와 포인트
		//SSegmentBound lay({ 0,1,0 }, { 0,-2,0 });
		//lay.check2D_XY(glm::vec3(1, 0.5, 0));
		//lay.check2D_XY(glm::vec3(0, -2, 0));

		// 세그먼트와 스피어
		//SSphereBound sph({ 0, 0, 0 }, 2.f);
		//SSegmentBound lay2({ 3,0,0 }, { -3,0,0 });
		//SSegmentBound lay3({ 3,2.1f,0 }, { -3,2.1f,0 });
		//SSegmentBound lay4({ 3,1.9f,0 }, { -3,1.9f,0 });
		//SSegmentBound lay5({ -3,0,0 }, { 3,0,0 });

		//lay2.check(sph, out);
		//lay3.check(sph, out);
		//lay4.check(sph, out);
		//lay5.check(sph, out);

		// 세그먼트와 트라이, 쿼드
		//SSegmentBound layT({ 0.5,0.5,3 }, { 0.5,0.5,-3 });
		//STriangleBound tri({ -1, 0, 0 }, { 1, -1, 0 }, { 1, 1, 0 });
		//STriangleBound triNoCross({ -3, 0, 0 }, { -1, -1, 0 }, { -1, 1, 0 });
		//rr = layT.check(tri, out);
		//rr = layT.check(triNoCross, out);

		//SQuadBound quad({ -1, -1, 0 }, { 1, -1, 0 }, { 1, 1, 0 }, { -1, 1, 0 });		
		//SQuadBound quadNoCross({ -3, -3, 0 }, { -1, -3, 0 }, { -1, -1, 0 }, { -3, -1, 0 });
		//rr = layT.check(quad, out);
		//rr = layT.check(quadNoCross, out);
		
		// 세그먼트와 큐브
		//SSegmentBound layT({ 0.5,0.5,3 }, { 0.5,0.5,-3 });
		//SCubeBound cube({ 0, 0, 0 }, { 2, 2, 2 }, { 0,0,0 });
		//rr = layT.check(cube, out);

		//layT = SSegmentBound({ 0.5,0.5,-3 }, { 0.5,0.5,+3 });
		//rr = layT.check(cube, out);

		//layT = SSegmentBound({ 3,0.5,0.5 }, { -3,0.5,0.5 });
		//rr = layT.check(cube, out);

		// 포인트와 스피어, 큐브
		//SPointBound point({ -4,0,0 });
		//SSphereBound spr_pt({ 0,0,0 }, 3.f);
		//SCubeBound cub_pt({ 0, 0, 0 }, { 3, 3, 3 }, { 0,0,0 });
		//rr = spr_pt.check(point);
		//rr = cub_pt.check(point);

		// 스피어와 스피어, 플레인,트라이,쿼드
		//SSphereBound spr_all({ 2,0,0 }, 3.f);
		//SSphereBound spr2({ 6,0,0 }, 3.f);
		//SSphereBound spr3({ -4,0,0 }, 3.f);
		//rr = spr_all.check(spr2, outSeg);
		//rr = spr2.check(spr_all, outSeg);
		//rr = spr_all.check(spr3, outSeg);

		//spr_all = SSphereBound({ 1,1,1 }, 3.f);
		//SPlaneBound plane({ 0,0,1 }, { 0,0,2 });
		//STriangleBound tri({ -1, -1.9f, 0 }, { 1, -1.9f, +1 }, { 1, -1.9f, -1 });
		//SQuadBound quad({ -1, -1, -1 }, { 1, -1, -1 }, { 1, 1, -1 }, { -1, +1, -1 });

		//rr = spr_all.check(plane, outSeg);
		//rr = spr_all.check(tri, outSeg);	// 선분과 교차
		//rr = spr_all.check(quad, outSeg);	// 선분과 교차

		//quad = SQuadBound({ -10, -10, 0 }, { 10, -10, 0 }, { 10, 10, 0 }, { -10, +10, 0 });
		//rr = spr_all.check(quad, outSeg);	// 도형의 면과 교차

		//// 스피어와 큐브
		//spr_all = SSphereBound({ 0,0,0 }, 3.f);

		//// 면과의 교차 체크 - Y -2 ~ -6
		//SCubeBound cube({ 0, -4, 0 }, { 20, 4, 20 }, { 0,0,0 });
		//rr = spr_all.check(cube, outSeg);
		//
		//// 선분과의 교차 체크 - X 2 ~ 6, / Y 0 ~ -4 / Z -5 ~ 5
		//cube = SCubeBound({ 4, -2, 0 }, { 4, 4, 10 }, { 0,0,0 });
		//rr = spr_all.check(cube, outSeg);

		//SCubeBound cube({ 0, 0, 0 }, { 6, 6, 6 }, { 0,0,0 });
		//SCubeBound cube2({ 4, 4, 4 }, { 6, 6, 6 }, { 0,0,0 });

		//rr = cube.check(cube2, outSeg);

		//cube2 = SCubeBound({ 7, 7, 7 }, { 6, 6, 6 }, { 0,0,0 });
		//rr = cube.check(cube2, outSeg);
		//}

		engine_ = new nsGE::CEngineBase(this);
		gameSettings_ = new nsGE::SGEGameSettings();

		// 테스트 코드
		gameSettings_->gameProjectName_ = Globals::GetGameProjectName();
		gameSettings_->gameProjectRootPath_ = Globals::GetGameProjectRoot();

#ifdef EDITOR_WORKING
		// 테스트 코드
		//CSharPtr<CAsset<CActor>> test = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(wtext("/actor/monkey.asset")));
		//auto testActor = new CStaticMeshActor(this);
		//testActor->CopyFromTemplate(test->getContentClass());

		CSharPtr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAsset<CMap>(Globals::GetGameAssetPath(wtext("/map/testMap.asset")));
		currentMap_ = new CMap(this);
		currentMap_->CopyFromTemplate(testMapTemplate->getContentClass());

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

				CCollideActor* rayActor = &currentMap_->SpawnActorVARIETY<CCollideActor>(true, currentMap_,
					ECheckCollideRule::NEAREST, false, 
					[this](class CActor* SRC, class CActor* TAR, const class CBoundComponent* SRC_BOUND, const class CBoundComponent* TAR_BOUND, const glm::vec3& COLL_POS)
					{
						CCollideActor* pointActor = &currentMap_->SpawnActorVARIETY<CCollideActor>(true, currentMap_);

						auto prefab = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabPointActor.asset")));
						pointActor->CopyFromTemplate(prefab->getContentClass());
						{
							// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
							pointActor->getTransform().Translate(COLL_POS);
						}

						currentMap_->FinishSpawnActor(*pointActor);
						
						pointActor->SetLifeTick(300);
					});

				auto prefab = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(wtext("/actor/prefabRayActor.asset")));
				rayActor->CopyFromTemplate(prefab->getContentClass());
				{	// 얘는 단독 액터니까 이렇게 직접 트랜스폼 해줘야한다
					// rayCompo 를 조작하는 게 아니고 rayActor 를 조작하고 있음에 주의!

					float rayLength = engine_->GetRenderingEngine()->GetCamera()->GetZFar();
					//rayCompo->SetBoundData(rayLength, ray_direction);
					rayActor->getTransform().Translate(ray_origin);
					rayActor->getTransform().Scale({ 0.1f, 0.1f, 0.1f });	// 여기 - GetOBB 를 위하여 약간의 두께를 갖게 했다, 이거 생각해봐야한다, 레이의 입장에서는 xy 크기는 0인게 맞지만 obb 로 역할하려면 BoundEpsilon 만큼은 있어야하므로...
					rayActor->getTransform().Scale(nsRE::TransformConst::DefaultAxis_Front, rayLength);
					rayActor->getTransform().RotateQuat(ray_direction);
				}
				currentMap_->FinishSpawnActor(*rayActor);

				auto targets = rayActor->QueryCollideCheckTargets();
				rayActor->ProcessCollide(targets);
				rayActor->SetLifeTick(100);
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
		if (currentMap_->IsBeganPlay() == false)
		{
			currentMap_->BeginPlay();
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
