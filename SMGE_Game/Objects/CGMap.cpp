#include "CGmap.h"
#include "../../SMGE/GECommonIncludes.h"
#include "../Assets/CGAssetManager.h"

namespace MonoMaxGraphics
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

	SGRefl_Map::SGRefl_Map(CGMap& map) :
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

		actorReflLayers_.resize(2);	// 테스트 코드

		// 이거긴 한데 이렇게 쓰면 auto 를 못쓴다...
		//using REFL_CVECTOR_FUNC = void(CVector<TupleVarName_VarType_Value>& variableSplitted, size_t childKey);
		auto FuncSpawnActor = [this](auto& variableSplitted, size_t childKey)
		{
			// 여기서 SGRefl_Actor 가 생성되려면 CGActor & 가 필요하다

			// 그러므로 SGRefl_Map 의 = 가 실행되려면 먼저
			// CGMap 에 CGActor 들이 인스턴싱 되어있어야한다
			// SGRefl_Map 이 CGMap 에 액터 생성을 시킨 후 연결해야한다 - 일단은 이렇게 구현해보자!
			CGActor loader;

			// 정확한 액터의 클래스명을 얻는다
			auto backupCursor = variableSplitted.cursor();
			loader.getReflection() = variableSplitted;
			loader.setActorStaticTag("ttttt");

			CWString actorAssetPath = CGAssetManager::FindAssetFilePathByClassName(loader.getClassName());
			if (SMGEGlobal::IsValidPath(actorAssetPath) == true)
			{
				// 1. 애셋을 이용하여 맵에 액터 스폰하기
				CSharPtr<CGAsset<CGActor>>& actorTemplate = CGAssetManager::LoadAsset<CGActor>(actorAssetPath);

				// 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				// 이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??

				CGActor& actorA = outerMap_->SpawnDefaultActor(*actorTemplate->getContentClass(), false);

				// 2 단계 - 맵에 저장된 값으로 배치시킨다
				variableSplitted.setCursor(backupCursor);
				actorA.getReflection() = variableSplitted;

				outerMap_->ArrangeActor(actorA);

				// 여기선 아직 this->actorLayers_ 에는 등록이 안되었다, 저 밑에 3단계에서 처리한다
			}
		};

		ReflectionUtils::FromCVector(actorReflLayers_[0], variableSplitted, FuncSpawnActor);
		ReflectionUtils::FromCVector(actorReflLayers_[1], variableSplitted, FuncSpawnActor);

		// 3단계 - 맵과 나를 레퍼런스로 연결한다
		linkActorReferences();

		return *this;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CGMap::CGMap() : CGObject()
	{
		className_ = wtext("SMGE_Game::CGMap");
		CGCtor();
	}

	CGMap::CGMap(const CGMap& templateInst) : CGMap()
	{
		CopyFromTemplate(templateInst);
	}

	void CGMap::CGCtor()
	{
		Super::CGCtor();

		actorLayers_.resize(etoi(EActorLayer::Max));
		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	SGReflection& CGMap::getReflection()
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

	CGActor& CGMap::SpawnDefaultActor(const CGActor& templateActor, bool isDynamic)
	{
		auto newActor = MakeSharPtr<CGActor>(templateActor);

		if (isDynamic == true)
		{	// ActorKeyGenerator
			static TActorKey ActorKeyGenerator = 0;
			newActor->actorKey_ = ++ActorKeyGenerator;
		}

		auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));

		rb->OnAfterSpawned(this, isDynamic);
		return *rb;
	}

	CGActor& CGMap::ArrangeActor(CGActor& targetActor)
	{
		targetActor.OnAfterArranged(this);

		if (isStarted_ == true)
		{
			targetActor.BeginPlay();
		}

		return targetActor;
	}

	void CGMap::StartToPlay()
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
