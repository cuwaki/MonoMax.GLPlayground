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


	// 하던 일 정리
	// 다중 컨테이너를 다룰 수 있게 해야한다 L"CVector<CVector<SGRefl_Actor>>";
	// template 으로 할지 runtime 에 할지 흠흠... dimension 을 이용하여 runtime 에 하는 게 나을 것 같다
	// 그리고 그게 끝나맨 매크로로 묶어줘야한다 ret += _TO_REFL_CONTAINER(CVector<SGRefl_Actor>, actorLayers_[1]);
	// 당연히 다중 컨테이너 읽는 부분도 수정되어야한다
	template<typename CT, typename KT>
	CWString ToCWString(const CT& cont, const CWString& contTypeName, const CWString& contVarName, std::optional<KT> contKeyForParent)
	{
		CWString ret;
		ret += contVarName;
		if (contKeyForParent.has_value())
			ret += L"[" + ToTCHAR(*contKeyForParent) + L"]";
		ret += SGReflection::META_DELIM;
		
		ret += contTypeName;
		ret += SGReflection::META_DELIM;

		ret += ToTCHAR(cont.size());
		ret += SGReflection::VARIABLE_DELIM;

		size_t dimension = std::count(contTypeName.begin(), contTypeName.end(), '<');

		if (dimension > 1)
		{
			// 다중 컨테이너
			//for (const auto& child : cont)
			//{
			//	CWString childContTypeName = L"CVector<SGRefl_Actor>";	// 자동으로 만들어야함
			//	CWString childContVarName = L"[i]";	// 자동으로 만들어야함, 맵 같은 경우에는 따로 처리해줘야겠다

			//	// 여기서 이렇게 자식들에 대해서 또 내려가야한다
			//	//ret += ToCWString(child, childContTypeName, childContVarName, 0);
			//}
		}
		else
		{	// 마지막이다
			for (const auto& it : cont)
			{
				ret += SCast<CWString>(it);
			}
		}

		return ret;
	}

	SGRefl_Map::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += ToCWString(actorReflLayers_[0], L"CVector<SGRefl_Actor>", L"actorLayers_[0]", std::optional<size_t>{});
		ret += ToCWString(actorReflLayers_[1], L"CVector<SGRefl_Actor>", L"actorLayers_[1]", std::optional<size_t>{});

		return ret;
	}


	template<typename CT, typename VS, typename FUNC>
	void FromCWString(CT& cont, VS& variableSplitted, FUNC& func)
	{
		size_t contSize = 0;
		CWString& size_str = std::get<Tuple_Value>(*variableSplitted.cursor());
		ReflectionUtils::FromREFL(contSize, size_str);

		variableSplitted.cursorNext();	// [2] = (L"actorLayers_[1]", L"CVector<SGRefl_Actor>", L"2") 를 먹어치운다

		if (contSize == 0)
			return;

		cont.clear();
		cont.reserve(contSize);

		for (size_t i = 0; i < contSize; i++)
		{
			func(variableSplitted, i);
		}
	}

	SGReflection& SGRefl_Map::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		//// 다중 컨테이너 읽을 때 
		//// actorLayers_.resize
		//	// actorLayers_[0
		//		// actorLayers_[0.resize
		//			// CGActors
		//	// actorLayers_[1
		//		// actorLayers_[0.resize
		//			// CGActors
		//
		////_FROM_REFL(actorKey_, variableSplitted);
		actorReflLayers_.resize(2);	// 테스트 코드

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

			CWString actorAssetPath = CGAssetManager::FindAssetFilePathByClassName(loader.getClassName());
			if (SMGEGlobal::IsValidPath(actorAssetPath) == true)
			{
				// 1. 애셋을 이용하여 맵에 액터 스폰하기
				CSharPtr<CGAsset<CGActor>>& actorTemplate = CGAssetManager::LoadAsset<CGActor>(actorAssetPath);

				// 실제 액터의 스폰이 리플렉션 단계에서 일어나게 된다... 구조상 좀 아쉬운 부분이다!
				// 이런 것 때문에 언리얼의 레벨도 특수한 방법이 들어가 있다는게 아닌가 싶은??

				CGActor& actorA = outerMap_->SpawnDefaultActor(*actorTemplate->getContentClass(), false);	// 배치

				// 2 단계 - 맵에 저장된 값으로 오버라이드시킨다
				variableSplitted.setCursor(backupCursor);
				actorA.getReflection() = variableSplitted;

				// 여기선 아직 this->actorLayers_ 에는 등록이 안되었다, 저 밑에 3단계에서 처리한다
			}
		};

		FromCWString(actorReflLayers_[0], variableSplitted, FuncSpawnActor);
		FromCWString(actorReflLayers_[1], variableSplitted, FuncSpawnActor);

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
		return *rb;
	}

	// 일단 안씀
	//CGActor& CGMap::OverrideActor(CGActor& arrangedActor)
	//{
	//	// 맵에 배치된 액터에 에디터에서 편집했던 값을 적용한다
	//	TReflectionStruct& mapReflStruct = SCast<TReflectionStruct&>(this->getReflection());

	//	const auto& cont = mapReflStruct.actorReflLayers_[EActorLayer::Game];
	//	auto act = GlobalUtils::FindIt(cont, arrangedActor.actorKey_);
	//	if (GlobalUtils::IsFound(cont, act))
	//	{
	//		SGStringStreamIn strIn(*act);
	//		strIn >> arrangedActor.getReflection();
	//	}
	//	return arrangedActor;
	//}

	void CGMap::Activate()
	{
		// 액터들 스폰/오버라이드 까지 끝났고 이제 맵을 작동시킨다
	}
};
