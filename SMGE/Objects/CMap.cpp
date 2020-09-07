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

		actorLayersREFL_.resize(2);	// 테스트 코드

		// 이거긴 한데 이렇게 쓰면 auto 를 못쓴다...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// 여기서 SGRefl_Actor 가 생성되려면 CActor & 가 필요하다

			// 그러므로 SGRefl_Map 의 = 가 실행되려면 먼저
			// CMap 에 CActor 들이 인스턴싱 되어있어야한다
			// SGRefl_Map 이 CMap 에 액터 생성을 시킨 후 연결해야한다 - 일단은 이렇게 구현해보자!

			CActor loader(nullptr);	// 액터의 애셋 경로를 얻는다
			auto backupCursor = variableSplitted.cursor();
			loader.getReflection() = variableSplitted;

			auto rootAssetPath = nsGE::CGameBase::Instance->PathAssetRoot();
			
			CWString actorAssetPath = loader.getReflectionFilePath();
			if (Path::IsValidPath(actorAssetPath) == true)
			{
				// 1. 애셋을 이용하여 맵에 액터 스폰하기 - RTTI
				CSharPtr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAsset<CActor>(rootAssetPath + actorAssetPath);

				// 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				// 이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??

				CActor& actorA = outerMap_.SpawnDefaultActor<CActor>(false, *actorTemplate->getContentClass());

				// 2 단계 - 맵에 저장된 값으로 배치시킨다
				variableSplitted.setCursor(backupCursor);
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
		className_ = wtext("SMGE::CMap");
		Ctor();
	}

	CMap::CMap(CObject* outer, const CMap& templateInst) : CMap(outer)
	{
		CopyFromTemplate(templateInst);
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
		for (auto it = actors.begin(); it != actors.end(); ++it)
		{
			auto& actor = (*it);
			if (actor->IsPendingKill())
			{
				actor->EndPlay();
				actor = nullptr;

				if (actors.size() > 1)
					actors.erase(it--);
				else
				{
					actors.erase(it);
					break;
				}
			}
		}
	}

	void CMap::Tick(float timeDelta)
	{
		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			sptrActor->Tick(timeDelta);
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
			reflMap_->SGRefl_Map::SGRefl_Map(*this);
		}

		return *reflMap_.get();
	}

	TActorKey CMap::DynamicActorKey = 3332;	// 테스트 코드

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

		if (isStarted_ == true)
		{
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

	void CMap::StartToPlay()
	{
		if (isStarted_ == true)
			throw SMGEException(wtext("CMap already activated"));

		isStarted_ = true;

		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			sptrActor->BeginPlay();
		}
	}

	void CMap::FinishPlaying()
	{
		if (isStarted_ == false)
			return;

		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			sptrActor->EndPlay();
		}

		actorLayers_.clear();
		isStarted_ = false;
	}
};
