#include "SPPKGame.h"
#include "Objects/CActor.h"
#include "Assets/CAsset.h"
#include "Assets/CAssetManager.h"

// 테스트 코드 - 테스트를 위한 기본 코드
#include "CEngineBase.h"
#include "Objects/CMap.h"
#include "Assets/CResourceModel.h"
#include "Components/CPointComponent.h"
#include "Components/CSegmentComponent.h"
#include "Objects/CCollideActor.h"
#include "Assets/CAssetManager.h"
#include "Assets/CAsset.h"
#include "Objects/CStaticMeshActor.h"
#include "CBoundCheck.h"

void testCppStudy();	// 테스트 코드 - cpp 스터디

namespace SMGE
{
	namespace Globals
	{
		// 테스트 코드 - sppk 를 위한 하드코딩
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
			ReplaceInline(ret, wtext("/Debug"), wtext(""));
			ReplaceInline(ret, wtext("/Release"), wtext(""));
			ReplaceInline(ret, wtext("/Shipping"), wtext(""));
			return ret;
		}
#else
		CWString GetGameProjectRoot()
		{
			return Path::GetNormalizedPath(Path::GetDirectoryCurrent());
		}
#endif
	}

	SPPKGame::SPPKGame(CObject* outer) : CGameBase(outer)
	{
		Initialize();

		Globals::GCurrentGame = this;
	}

	SPPKGame::~SPPKGame()
	{
		currentMap_->FinishPlaying();

		Globals::GCurrentGame = nullptr;
		Globals::GCurrentMap = nullptr;
	}

#define EDITOR_WORKING

	void SPPKGame::Initialize()
	{
		// 테스트 코드 - cpp 공부 ㅋㅋ
		//::testCppStudy();

		engine_ = std::make_unique<CEngineBase>(this);
		gameSettings_ = std::make_unique<SGEGameSettings>();

		gameSettings_->gameProjectName_ = Globals::GetGameProjectName();
		gameSettings_->gameProjectRootPath_ = Globals::GetGameProjectRoot();
	}

	void SPPKGame::Tick(float dt)
	{
		Super::Tick(dt);

#ifdef EDITOR_WORKING
		if (currentMap_ == nullptr)
		{	// 테스트 코드 ㅡ 개발용
			// 테스트 코드 - 스태틱 메시 애셋 로드 테스트
			//std::shared_ptr<CAsset<CActor>> test = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(wtext("/actor/monkey.asset")));
			//auto testActor = new CStaticMeshActor(this);
			//testActor->CopyFromTemplate(test->getContentClass());

			std::shared_ptr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAssetDefault<CMap>(Globals::GetGameAssetPath(wtext("/map/testMap.asset")));
			currentMap_ = std::make_unique<CMap>(this);
			
			// 테스트 코드 ㅡ 맵 리플렉션 전역 변수 안쓰게 수정 필요
			Globals::GCurrentMap = currentMap_.get();

			currentMap_->CopyFromTemplate(testMapTemplate->getContentClass());

			if (currentMap_->IsBeganPlay() == false)
			{
				currentMap_->BeginPlay();
			}
		}

		currentMap_->Tick(dt);
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
			std::shared_ptr<CAsset<CActor>> loadingActorTemplate = CAssetManager::LoadAssetDefault<CActor>(actorAssetPath);
			auto loadedActor = new CActor(this, *loadingActorTemplate->getContentClass());
		// 여기까지 액터 저장 및 로드



		//// 맵 템플릿 애셋 테스트 코드 - CActor 를 맵에 저장하기 - 맵에 배치한 후 수정한 값으로, 맵이 로드된 후 액터가 배치된 후 이 값으로 덮어씌우게 된다
		//std::shared_ptr<CAsset<CActor>> testActorTemplate = CAssetManager::LoadAssetDefault<CActor>(assetRoot + wtext("testActorTemplate.asset"));
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
		std::shared_ptr<CAsset<CResourceModel>> amA = CAssetManager::LoadAssetDefault<CResourceModel>(assetRoot + wtext("/models/cube/cube.asset"));
		애셋모델 저장하기 */

	//	// 맵 로드하고 액터들 다시 복구하기
	//	std::shared_ptr<CAsset<CMap>> testMapTemplate = CAssetManager::LoadAssetDefault<CMap>(assetRoot + wtext("/map/testMapTemplate.asset"));
	//	CMap loadedMap(*testMapTemplate->getContentClass());
	//	loadedMap.Activate();
	//	// }
#endif
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cpp study - 마구잡이 공부 영역 나중에 날릴 것
#include <array>

// https://stackoverflow.com/questions/34143943/how-to-fill-const-stdarraysize-t-n-with-values-based-on-function
// array 등을 인덱스에 의거하여 초기화하는 함수 1
template<typename T, size_t N, typename Fn>
std::array<T, N> init_from_f(Fn&& f)
{
	Fn&& runF = std::forward<Fn>(f);

	std::array<T, N> ret;
	for (size_t i = 0; i < N; ++i)
		ret[i] = runF(i);
	return ret;
}

// array 등을 인덱스에 의거하여 초기화하는 함수 2
template<size_t... ns, typename Fn>
auto fill_helper(std::integer_sequence<size_t, ns...>, Fn&& fn)
	-> std::array<decltype(fn(std::size_t())), sizeof...(ns)>	// 후행 리턴값 정의가 반드시 필요하다
{
	return { fn(ns)... };	// ... 을 이용한 호출로 코드에서 이니셜라이저 리스트를 만든다 - 신기하네~
}

template<size_t N, typename Fn>
auto fill(Fn&& fn)
{
	using make_seq = std::make_integer_sequence<size_t, N>;

	// fill_helper 타입을 이렇게 만든다
	// fill_helper<0, 1, 2, 3, 4, class <lambda_116f7dd254db719f50c299bdfc1a6223>&>(

	// 그리고 이렇게 호출한다
	//	std::integer_sequence<unsigned __int64, 0, 1, 2, 3, 4> __formal, ? 
	//	testCppStudy@@$$FYAXXZ::__l2::<lambda_116f7dd254db719f50c299bdfc1a6223>&fn)
	return fill_helper(make_seq(), std::forward<Fn>(fn));
}

void testCppStudy()
{
	auto c6 = init_from_f<char, 6>([](auto i) { return i + 4; });

	auto plus5 = [](size_t index)
	{
		return index + 5;
	};
	auto const as = fill<5>(plus5);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

