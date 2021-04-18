#include "CMap.h"
#include "../GECommonIncludes.h"
#include "../Assets/CAssetManager.h"
#include "../CEngineBase.h"
#include "../CGameBase.h"
#include "CCameraActor.h"

#define ENABLE_FRUSTUM_CULLING
#define ENABLE_OCTREE
//#define ENABLE_OCTREE_WITH_AABB	/* 옥트리에 크기로 등록하기 */

namespace SMGE
{
	namespace Globals
	{
		CMap* GCurrentMap;
	}

	SGRefl_Map::SGRefl_Map(CMap& map) :
		outerMap_(map),
		SGReflection(map)
	{
		linkINST2REFL();
	}

	void SGRefl_Map::linkINST2REFL()
	{
		mapActorsRefl_.clear();

		ReflectionUtils::clearAndEmplaceBackINST2REFL(mapActorsRefl_, outerMap_.mapActorsW_);
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ReflectionUtils::ToCVector(mapActorsRefl_, L"CVector<SGReflection&>", L"mapActorsW_", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		// 이거긴 한데 이렇게 쓰면 auto 를 못쓴다...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// 여기서 SGRefl_Actor 가 생성되려면 CActor & 가 필요하다

			// 그러므로 SGRefl_Map 의 = 가 실행되려면 먼저
			// CMap 에 CActor 들이 인스턴싱 되어있어야한다
			// SGRefl_Map 이 CMap 에 액터 생성을 시킨 후 연결해야한다 - 일단은 이렇게 구현해보자!

			CString actorClassRTTIName = ReflectionUtils::GetClassRTTIName(variableSplitted);
			CWString actorAssetPath = ReflectionUtils::GetReflectionFilePath(variableSplitted);

			if (Path::IsValidPath(actorAssetPath) == true)
			{
				// 여기 -
				// 1. 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				//		이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??
				// 2. 여기 - 최적화 - 템플릿으로써 사용될 때도 액터 스폰이 일어나게 된다
				//		낭비인데... 방법을 찾자! - 실제 인스턴스에게 std::move 를 시켜줄까?? 또는 템플릿의 경우에는 스폰을 안하면 되잖아?

				// 1. 애셋을 이용하여 맵에 액터 스폰하기 - RTTI
				//std::shared_ptr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				//CActor& actorA = outerMap_.SpawnDefaultActor(actorClassRTTIName, false, &outerMap_, actorTemplate->getContentClass());

				// 1. 디폴트로 생성하고
				const auto system = Globals::GCurrentGame->GetEngine()->GetSystem();
				auto actorA = &system->StartSpawnActorDEFAULT(&outerMap_, actorClassRTTIName, false, Args_START &outerMap_);

				// 2. 애셋 덮어씌우고
				auto actorTemplate = CAssetManager::LoadAssetDefault<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				actorA->CopyFromTemplate(actorTemplate->getContentClass());

				// 3. 맵에 저장된 값으로 한번 더 덮어쓴다 - 위치나 맵에서 박은 콤포넌트 등등
				actorA->getReflection() = variableSplitted;
				system->FinishSpawnActor(&outerMap_, actorA);

				// 여기선 아직 this->mapActorsW_ 에는 등록이 안되었다, 저 밑에 3단계에서 처리한다
			}
			else
			{
				throw SMGEException(wtext("CMap : FuncSpawnActor : bad filepath - ") + actorAssetPath);
			}
		};

		ReflectionUtils::FromCVector(mapActorsRefl_, variableSplitted, FuncSpawnActor);

		// 3단계 - 맵과 나를 레퍼런스로 연결한다
		linkINST2REFL();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMap::CMap(CObject* outer) : CObject(outer),
#ifdef ENABLE_FRUSTUM_CULLING
		isFrustumCulling_(true)
#else
		isFrustumCulling_(false)
#endif
	{
		Ctor();
	}

	void CMap::Ctor()
	{
		Super::Ctor();

		mapActorsW_.reserve(100);
	}

	void CMap::Tick(float timeDelta)
	{
		cameraFrustumCulling();

		for (auto& actor : mapActorsW_)
		{
			actor->Tick(timeDelta);	// 여기 - 나에게만 관련된 틱과 다른 액터와 관련된 틱을 분리하면 멀티쓰레드 처리를 할 수 있겠다 / 또는 로직 처리용 컴포넌트를 만들고 컴포넌트별로 멀티쓰레더블과 아닌걸로 나눌까?
			const auto isDirty = actor->getTransform().IsDirty();	// 여기 - 규칙 추가 - 액터의 트랜스폼 더티는 Tick 에서만 이뤄져야한다!
			actor->AfterTick(timeDelta);

#ifdef ENABLE_OCTREE
			if (isDirty)
			{	// 여기 - 최적화 - 일단 무식하게 한다
				RemoveActorFromOctree(actor);
				mapOctree_.AddByPoint(actor, actor->getTransform().GetFinalPosition());

				// 역시 일단 무식하게 한다
				if (actor->GetMainBound() != nullptr)
				{
					actor->GetMainBound()->CacheAABB();
				}
			}
#endif
		}
	}

	bool CMap::IsTemplate() const
	{
		return GetOuter() == nullptr;	// 템플릿이 아닌 경우 Game 이 outer 이다
	}

	SGReflection& CMap::getReflection()
	{
		if (reflMap_.get() == nullptr)
		{
			reflMap_ = std::make_unique<TReflectionStruct>(*this);
		}
		else
		{	// 차후 this->mapActorsW_ 에 변경이 있는 경우에만 바뀌어야한다
			//reflMap_->SGRefl_Map::SGRefl_Map(*this);	// 201003 이거 왜 이렇게 했더라? 구조적으로 이렇게 하면 안되게 되어서 주석으로 막음
		}

		return *reflMap_.get();
	}

	CActor* CMap::FindActor(TActorKey ak)
	{
		return nullptr;
	}

	const CVector<CActor*>& CMap::GetMapActors() const
	{
		return mapActorsW_;
	}

	CVector<CActor*> CMap::QueryActors(const SAABB& aabb) const
	{
#ifdef ENABLE_OCTREE
		return mapOctree_.QueryValuesByCube(aabb.min(), aabb.max());
#else
		return mapActorsW_;
#endif
	}

	void CMap::changeCurrentCamera(CCameraActor* camA)
	{
		if (currentCamera_ == camA)
			return;

		if (currentCamera_)
			currentCamera_->onChangedCurrent(false);

		currentCamera_ = camA;
		cullingCamera_ = camA;
 
		if(currentCamera_)
			currentCamera_->onChangedCurrent(true);
	}

	void CMap::OnPostBeginPlay()
	{
#ifdef ENABLE_OCTREE
		mapOctree_.Create("mapOctree_", MapConst::MaxX, MapConst::MaxY, MapConst::MaxZ, MapConst::OctreeLeafWidth);

		// 여기 - 규칙 - BeginPlay( 에서는 다른 액터를 쿼리하며 안된다! 옥트리에 아직 등록이 안되었기 때문!
		for (auto& actor : mapActorsW_)
		{
			actor->getTransform().RecalcFinal();
			mapOctree_.AddByPoint(actor, actor->getTransform().GetFinalPosition());	// 여기 - 크기로 등록하기, 모든 AddByPoint에! ENABLE_OCTREE_WITH_AABB
		}
#endif
		currentCamera_ = nullptr;

		// 현재 카메라 설정
		for (auto& actor : mapActorsW_)
		{
			auto firstCamActor = dynamic_cast<CCameraActor*>(actor);
			if (firstCamActor)
			{
				changeCurrentCamera(firstCamActor);
				break;
			}
		}

		// 테스트 코드 - 프러스텀 컬링 시각화
		for (auto& actor : mapActorsW_)
		{
			auto testCamActor = dynamic_cast<CCameraActor*>(actor);
			if (testCamActor && testCamActor->getActorStaticTag() == "testCamera")
			{
				cullingCamera_ = testCamActor;
				break;
			}
		}

		if (isFrustumCulling_)
		{	// 초기 프러스텀 컬링
			for (auto& actor : mapActorsW_)
				actor->SetRendering(false, true);

			cameraFrustumCulling();
		}
		else
		{
			for (auto& actor : mapActorsW_)
				actor->SetRendering(true, true);
		}
	}

	void CMap::cameraFrustumCulling()
	{
#ifdef ENABLE_FRUSTUM_CULLING
		if (isFrustumCulling_ == false || cullingCamera_ == nullptr)
			return;

		for (auto actor : oldActorsInFrustum_)
		{
			if (actor)
				actor->SetRendering(false, true);
		}
		
		oldActorsInFrustum_ = QueryActors(cullingCamera_->GetFrustumAABB());	// 최적화 - ref out 으로 처리할 것

		// 최적화 - 나중에 멀티쓰레드로 바꿀 것
		for (auto& actorPtr : oldActorsInFrustum_)
		{
			//// 액터 기준 처리 - 이러면 빠르고 편한데 액터에 어태치된 컴포넌트들이 제대로 처리가 안된다
			//const auto mainBound = actorPtr->GetMainBound();
			//if (mainBound == nullptr)
			//{
			//	actorPtr->SetRendering(true);	// 컬링이 불가능하므로 무조건 그린다
			//	actorPtr = nullptr;
			//}
			//else
			//	actorPtr->SetRendering(cullingCamera_->IsInOrIntersectWithFrustum(mainBound));

			// 드로 컴포넌트 기준 처리
			for (auto& comp : actorPtr->getAllComponents())
			{
				auto drawComp = dynamic_cast<CDrawComponent*>(comp);
				if (drawComp)
				{
					auto mbc = drawComp->GetMainBound();
					if (mbc)
					{	// 드로 콤포에 바운드가 있는 경우 - 그걸로 판단
					}
					else
					{	// 없는 경우 - 부모의 그것으로 판단
						mbc = actorPtr->GetMainBound();
					}

					const auto isRender = cullingCamera_->IsInOrIntersectWithFrustum(mbc);
					drawComp->SetRendering(isRender, false);

					// 여기 - 여기를 액터로부터 시작해서 자식 콤포들에 대해서 모두 돌아야한다 - cameraFrustumCulling 라는 함수를  액터, 콤포 체인으로 만들어서 한방에 트리 운행으로 처리하도록
					// 부모가 보이지 않는다고 자식이 보이지 말라는 법이 없다 (propagate)
					// 그리고 액터를 Transform 상속으로 해야할 것 같다 -> // 여기 수정 - 흠.... 이거 어떻게?
				}
			}
		}
#else
#endif
	}

	void CMap::BeginPlay()
	{
		if (isBeganPlay_ == true)
			throw SMGEException(wtext("CMap already activated"));

		isBeginningPlay_ = true;
		{
			for (auto& actor : mapActorsW_)
				actor->BeginPlay();
		}
		isBeginningPlay_ = false;

		OnPostBeginPlay();

		isBeganPlay_ = true;
	}

	void CMap::FinishPlaying()
	{
		if (isBeganPlay_ == false)
			return;

		for (auto& actor : mapActorsW_)
			actor->EndPlay();

#ifdef ENABLE_OCTREE
		mapOctree_.Clear();
#endif
		mapActorsW_.clear();
#ifdef ENABLE_FRUSTUM_CULLING
		oldActorsInFrustum_.clear();
#endif

		isBeganPlay_ = false;
	}

	class CCameraActor* CMap::GetCurrentCamera() const
	{
		return currentCamera_;
	}

	void CMap::RemoveActorFromOctree(CActor* actor)
	{
		if (actor == nullptr)
			return;

		actor->getTransform().RecalcFinal();

		auto maybeSuccess = mapOctree_.RemoveByPoint(actor, actor->getTransform().GetFinalPosition());
		if (maybeSuccess == false)	// 위치로 찾아보고 없으면 어쩔 수 없다 - 값으로 찾아야한다
		{
			MapOcTree::TValueIterator it;

			auto [xyNodes, zxNodes] = mapOctree_.HardQuery(actor);
#if _DEBUG || DEBUG
			// 이게 반드시 보장되어야한다
			const auto xySize = std::distance(xyNodes.begin(), xyNodes.end());
			const auto zxSize = std::distance(zxNodes.begin(), zxNodes.end());
	#ifdef ENABLE_OCTREE_WITH_AABB
			assert(zxSize == xySize);
	#else
			assert(xySize == 1);
			assert(zxSize == 1);
	#endif
#endif
			auto& xyNode = *xyNodes.begin();
			if (xyNode)
			{
				xyNode->FindValue(actor, it);
				xyNode->RemoveValue(it);
			}
			auto& zxNode = *zxNodes.begin();
			if (zxNode)
			{
				zxNode->FindValue(actor, it);
				zxNode->RemoveValue(it);
			}
		}
	}

	void CMap::ProcessPendingKill(CActor* actor)
	{
		if (actor == nullptr)
			return;

		actor->EndPlay();

#ifdef ENABLE_OCTREE
		RemoveActorFromOctree(actor);
#endif
#ifdef ENABLE_FRUSTUM_CULLING
		auto it = std::find(oldActorsInFrustum_.begin(), oldActorsInFrustum_.end(), actor);
		if (it != oldActorsInFrustum_.end())
			*it = nullptr;
#endif

		auto found = std::find(mapActorsW_.begin(), mapActorsW_.end(), actor);
#if defined(_DEBUG) || defined(DEBUG)
		assert(found != mapActorsW_.end() && "must be");
#endif
		mapActorsW_.erase(found);
	}

	void CMap::AddMapActor(CActor* actor)
	{
#if defined(_DEBUG) || defined(DEBUG)
		auto found = std::find(mapActorsW_.begin(), mapActorsW_.end(), actor);
		assert(found == mapActorsW_.end() && "never overlap");
#endif
		mapActorsW_.push_back(actor);

		if (isBeganPlay_ == true)
		{
			actor->BeginPlay();

#ifdef ENABLE_OCTREE
			actor->getTransform().RecalcFinal();
			mapOctree_.AddByPoint(actor, actor->getTransform().GetFinalPosition());
#endif

			if (isFrustumCulling_)
			{	// 안보임으로 시작하며 필요시 보이게 될 것
				actor->SetRendering(false, true);
			}
			else
			{
				actor->SetRendering(true, true);
			}
		}
	}
};
