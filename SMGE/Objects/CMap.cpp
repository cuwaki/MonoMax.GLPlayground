#include "CMap.h"
#include "../GECommonIncludes.h"
#include "../Assets/CAssetManager.h"
#include "../CGameBase.h"

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
		actorLayersREFL_.resize(outerMap_.actorLayers_.size());

		ReflectionUtils::clearAndEmplaceBackINST2REFL(actorLayersREFL_[EActorLayer::System], outerMap_.actorLayers_[EActorLayer::System]);
		ReflectionUtils::clearAndEmplaceBackINST2REFL(actorLayersREFL_[EActorLayer::Game], outerMap_.actorLayers_[EActorLayer::Game]);
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ReflectionUtils::ToCVector(actorLayersREFL_[0], L"CVector<SGRefl_Actor>", L"actorLayersREFL_[0]", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(actorLayersREFL_[1], L"CVector<SGRefl_Actor>", L"actorLayersREFL_[1]", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		actorLayersREFL_.resize(etoi(EActorLayer::Max));

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
				// 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				// 이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??

				// 1. 애셋을 이용하여 맵에 액터 스폰하기 - RTTI
				//CSharPtr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				//CActor& actorA = outerMap_.SpawnDefaultActor(actorClassRTTIName, false, &outerMap_, actorTemplate->getContentClass());

				// 1. 디폴트로 생성하고
				CActor& actorA = outerMap_.SpawnActorDEFAULT(actorClassRTTIName, false, &outerMap_);

				// 2. 애셋 덮어씌우고
				auto actorTemplate = CAssetManager::LoadAsset<CActor>(Globals::GetGameAssetPath(actorAssetPath));
				actorA.CopyFromTemplate(actorTemplate->getContentClass());

				// 3. 맵에 저장된 값으로 한번 더 덮어쓴다 - 위치나 맵에서 박은 콤포넌트 등등
				actorA.getReflection() = variableSplitted;

				outerMap_.FinishSpawnActor(actorA);

				// 여기선 아직 this->actorLayers_ 에는 등록이 안되었다, 저 밑에 3단계에서 처리한다
			}
			else
			{
				throw SMGEException(wtext("CMap : FuncSpawnActor : bad filepath - ") + actorAssetPath);
			}
		};

		ReflectionUtils::FromCVector(actorLayersREFL_[0], variableSplitted, FuncSpawnActor);
		ReflectionUtils::FromCVector(actorLayersREFL_[1], variableSplitted, FuncSpawnActor);

		// 3단계 - 맵과 나를 레퍼런스로 연결한다
		linkINST2REFL();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMap::CMap(CObject* outer) : CObject(outer)
	{
		Ctor();
	}

	void CMap::Ctor()
	{
		Super::Ctor();

		actorLayers_.resize(etoi(EActorLayer::Max));
		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	void CMap::ProcessPendingKills()
	{
		auto& actors = actorLayers_[EActorLayer::Game];
		if (actors.size() == 0)
			return;
		
		for (size_t i = actors.size() - 1; i > 0;)
		{
			auto& actor = actors[i];
			if (actor->IsPendingKill())
			{
				actorOctree_.RemoveByPoint(actor.get(), actor.get()->getLocation());
				actor->EndPlay();

				actor = nullptr;

				actors.erase(actors.begin() + i);
			}

			if (i == 0)
				break;
			else
				--i;
		}
	}

	void CMap::Tick(float timeDelta)
	{
		glm::vec3 oldLoc, newLoc;

		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			oldLoc = sptrActor->getLocation();

			sptrActor->Tick(timeDelta);

			newLoc = sptrActor->getLocation();
			if(oldLoc != newLoc)	// 여기 - 위치만 가지고 하면 안된다, 차후에 dirty 를 이용해서 업데이트 하도록 하자 / 옥트리 업데이트 말고도 aabb 업데이트도 해야한다
			{	// 일단 무식하게 한다
				actorOctree_.RemoveByPoint(sptrActor.get(), oldLoc);
				actorOctree_.AddByPoint(sptrActor.get(), newLoc);

				// 역시 일단 무식하게 한다
				if (sptrActor->GetMainBound() != nullptr)
				{
					sptrActor->GetMainBound()->CacheAABB();
				}
			}
		}
	}

	void CMap::Render(float timeDelta)
	{
		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			sptrActor->Render(timeDelta);
		}
	}

	SGReflection& CMap::getReflection()
	{
		if (reflMap_.get() == nullptr)
		{
			reflMap_ = MakeUniqPtr<TReflectionStruct>(*this);
		}
		else
		{	// 차후 this->actorLayers_ 에 변경이 있는 경우에만 바뀌어야한다
			//reflMap_->SGRefl_Map::SGRefl_Map(*this);	// 201003 이거 왜 이렇게 했더라? 구조적으로 이렇게 하면 안되게 되어서 주석으로 막음
		}

		return *reflMap_.get();
	}

	TActorKey CMap::DynamicActorKey = 3332;	// 디버깅할 때 티나라고 이상한 값에서 시작하게 해둠

	//CActor& CMap::SpawnDefaultActor(const CActor& templateActor, bool isDynamic)
	//{
	//	auto newActor = MakeSharPtr<CActor>(this, templateActor);

	//	if (isDynamic == true)
	//	{	// DynamicActorKey
	//		newActor->actorKey_ = DynamicActorKey++;
	//	}

	//	auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));

	//	rb->OnSpawnStarted(this, isDynamic);
	//	return *rb;
	//}

	CActor& CMap::FinishSpawnActor(CActor& targetActor)
	{
		targetActor.OnSpawnFinished(this);

		if (isBeganPlay_ == true)
		{
			actorOctree_.AddByPoint(&targetActor, targetActor.getLocation());
			targetActor.BeginPlay();
		}

		return targetActor;
	}

	CActor* CMap::FindActor(TActorKey ak)
	{
		return nullptr;
	}

	CSharPtr<CActor>&& CMap::RemoveActor(TActorKey ak)
	{
		return std::move(actorLayers_[EActorLayer::Game][0]);
	}

	const CVector<CSharPtr<CActor>>& CMap::GetActors(EActorLayer layer) const
	{
		return actorLayers_[layer];
	}

	CVector<CActor*> CMap::QueryActors(const SAABB& aabb) const
	{
		return actorOctree_.QueryValuesByCube(aabb.min(), aabb.max());
	}

	void CMap::BeginPlay()
	{
		if (isBeganPlay_ == true)
			throw SMGEException(wtext("CMap already activated"));

		isBeginningPlay_ = true;
		{
			actorOctree_.Create("actorOctree_", MapConst::MaxX, MapConst::MaxY, MapConst::MaxZ, MapConst::OctreeLeafWidth);

			for (auto& sptrActor : actorLayers_[EActorLayer::Game])
			{
				actorOctree_.AddByPoint(sptrActor.get(), sptrActor->getLocation());
				sptrActor->BeginPlay();
			}
		}
		isBeginningPlay_ = false;

		isBeganPlay_ = true;
	}

	void CMap::FinishPlaying()
	{
		if (isBeganPlay_ == false)
			return;

		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			actorOctree_.RemoveByPoint(sptrActor.get(), sptrActor->getLocation());
			sptrActor->EndPlay();
		}

		actorLayers_.clear();
		isBeganPlay_ = false;
	}
};
