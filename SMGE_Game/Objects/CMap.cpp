#include "CMap.h"
#include "GECommonIncludes.h"
#include "Assets/CAssetManager.h"

namespace SMGE
{
	template<typename T, typename U>
	CVector<T>& clearAndAssign(CVector<T>& left, const CVector<U>& right)
	{
		left.clear();
		left.reserve(right.size());

		for (int i = 0; i < right.size(); ++i)
		{
			left.emplace_back(right[i]);
		}

		return left;
	};

	SGRefl_Map::SGRefl_Map(CMap& map) :
		SGReflection(map)
	{
		outerMap_ = &map;
		linkActorReferences();
	}

	void SGRefl_Map::linkActorReferences()
	{
		actorReflLayers_.resize(outerMap_->actorLayers_.size());

		clearAndAssign(actorReflLayers_[EActorLayer::System], outerMap_->actorLayers_[EActorLayer::System]);
		clearAndAssign(actorReflLayers_[EActorLayer::Game], outerMap_->actorLayers_[EActorLayer::Game]);
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ReflectionUtils::ToCVector(actorReflLayers_[0], L"CVector<SGRefl_Actor>", L"actorLayers_[0]", std::optional<size_t>{});
		ret += ReflectionUtils::ToCVector(actorReflLayers_[1], L"CVector<SGRefl_Actor>", L"actorLayers_[1]", std::optional<size_t>{});

		return ret;
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		actorReflLayers_.resize(2);	// �׽�Ʈ �ڵ�

		// �̰ű� �ѵ� �̷��� ���� auto �� ������...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// ���⼭ SGRefl_Actor �� �����Ƿ��� CActor & �� �ʿ��ϴ�

			// �׷��Ƿ� SGRefl_Map �� = �� ����Ƿ��� ����
			// CMap �� CActor ���� �ν��Ͻ� �Ǿ��־���Ѵ�
			// SGRefl_Map �� CMap �� ���� ������ ��Ų �� �����ؾ��Ѵ� - �ϴ��� �̷��� �����غ���!
			CActor loader;

			// ��Ȯ�� ������ Ŭ�������� ��´�
			auto backupCursor = variableSplitted.cursor();
			loader.getReflection() = variableSplitted;
			loader.setActorStaticTag("ttttt");

			CWString actorAssetPath = CAssetManager::FindAssetFilePathByClassName(loader.getClassName());
			if (SMGEGlobal::IsValidPath(actorAssetPath) == true)
			{
				// 1. �ּ��� �̿��Ͽ� �ʿ� ���� �����ϱ�
				CSharPtr<CAsset<CActor>>& actorTemplate = CAssetManager::LoadAsset<CActor>(actorAssetPath);

				// ���� ������ ������ ���÷��� �ܰ迡�� �Ͼ�� �ȴ�... ������ �� �ƽ��� �κ��̴�!
				// �̷� �� ������ �𸮾��� ������ Ư���� ����� �� �ִٴ°� �ƴѰ� ����??

				CActor& actorA = outerMap_->SpawnDefaultActor(*actorTemplate->getContentClass(), false);

				// 2 �ܰ� - �ʿ� ����� ������ ��ġ��Ų��
				variableSplitted.setCursor(backupCursor);
				actorA.getReflection() = variableSplitted;

				outerMap_->ArrangeActor(actorA);

				// ���⼱ ���� this->actorLayers_ ���� ����� �ȵǾ���, �� �ؿ� 3�ܰ迡�� ó���Ѵ�
			}
		};

		ReflectionUtils::FromCVector(actorReflLayers_[0], variableSplitted, FuncSpawnActor);
		ReflectionUtils::FromCVector(actorReflLayers_[1], variableSplitted, FuncSpawnActor);

		// 3�ܰ� - �ʰ� ���� ���۷����� �����Ѵ�
		linkActorReferences();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CMap::CMap() : CObject()
	{
		className_ = wtext("SMGE_Game::CMap");
		CGCtor();
	}

	CMap::CMap(const CMap& templateInst) : CMap()
	{
		CopyFromTemplate(templateInst);
	}

	void CMap::CGCtor()
	{
		Super::CGCtor();

		actorLayers_.resize(etoi(EActorLayer::Max));
		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
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

	CActor& CMap::SpawnDefaultActor(const CActor& templateActor, bool isDynamic)
	{
		auto newActor = MakeSharPtr<CActor>(templateActor);

		if (isDynamic == true)
		{	// ActorKeyGenerator
			static TActorKey ActorKeyGenerator = 0;
			newActor->actorKey_ = ++ActorKeyGenerator;
		}

		auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));

		rb->OnAfterSpawned(this, isDynamic);
		return *rb;
	}

	CActor& CMap::ArrangeActor(CActor& targetActor)
	{
		targetActor.OnAfterArranged(this);

		if (isStarted_ == true)
		{
			targetActor.BeginPlay();
		}

		return targetActor;
	}

	void CMap::StartToPlay()
	{
		if (isStarted_ == true)
			throw SMGEException(wtext("cgmap already activated"));

		isStarted_ = true;

		for (auto& sptrActor : actorLayers_[EActorLayer::Game])
		{
			sptrActor->BeginPlay();
		}
	}
};
