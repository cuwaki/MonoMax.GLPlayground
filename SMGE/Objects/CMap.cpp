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

		actorLayersREFL_.resize(2);	// �׽�Ʈ �ڵ�

		// �̰ű� �ѵ� �̷��� ���� auto �� ������...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// ���⼭ SGRefl_Actor �� �����Ƿ��� CActor & �� �ʿ��ϴ�

			// �׷��Ƿ� SGRefl_Map �� = �� ����Ƿ��� ����
			// CMap �� CActor ���� �ν��Ͻ� �Ǿ��־���Ѵ�
			// SGRefl_Map �� CMap �� ���� ������ ��Ų �� �����ؾ��Ѵ� - �ϴ��� �̷��� �����غ���!

			CActor loader(nullptr);	// ������ �ּ� ��θ� ��´�
			auto backupCursor = variableSplitted.cursor();
			loader.getReflection() = variableSplitted;

			auto rootAssetPath = nsGE::CGameBase::Instance->PathAssetRoot();
			
			CWString actorAssetPath = loader.getReflectionFilePath();
			if (Path::IsValidPath(actorAssetPath) == true)
			{
				// 1. �ּ��� �̿��Ͽ� �ʿ� ���� �����ϱ� - RTTI
				CSharPtr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAsset<CActor>(rootAssetPath + actorAssetPath);

				// ���� ������ ������ ���÷��� �ܰ迡�� �Ͼ�� �ȴ�... ������ �� �ƽ��� �κ��̴�!
				// �̷� �� ������ �𸮾��� ������ Ư���� ����� �� �ִٴ°� �ƴѰ� ����??

				CActor& actorA = outerMap_.SpawnDefaultActor<CActor>(false, *actorTemplate->getContentClass());

				// 2 �ܰ� - �ʿ� ����� ������ ��ġ��Ų��
				variableSplitted.setCursor(backupCursor);
				actorA.getReflection() = variableSplitted;

				outerMap_.FinishSpawnActor(actorA);

				// ���⼱ ���� this->actorLayers_ ���� ����� �ȵǾ���, �� �ؿ� 3�ܰ迡�� ó���Ѵ�
			}
			else
			{
				throw SMGEException(wtext("CMap : FuncSpawnActor : bad filepath - ") + actorAssetPath);
			}
		};

		ReflectionUtils::FromCVector(actorLayersREFL_[0], variableSplitted, FuncSpawnActor);
		ReflectionUtils::FromCVector(actorLayersREFL_[1], variableSplitted, FuncSpawnActor);

		// 3�ܰ� - �ʰ� ���� ���۷����� �����Ѵ�
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
		{	// ���� this->actorLayers_ �� ������ �ִ� ��쿡�� �ٲ����Ѵ�
			reflMap_->SGRefl_Map::SGRefl_Map(*this);
		}

		return *reflMap_.get();
	}

	TActorKey CMap::DynamicActorKey = 3332;	// �׽�Ʈ �ڵ�

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
