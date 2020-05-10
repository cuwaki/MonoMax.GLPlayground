#include "CGMap.h"

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
		clearAndAssign(actorLayers_[EActorLayer::System], map.actorLayers_[EActorLayer::System]);
		clearAndAssign(actorLayers_[EActorLayer::Game], map.actorLayers_[EActorLayer::Game]);
	}

	CGMap::CGMap()
	{
		reflClassName_ = wtext("SMGE_Game::CGMap");
	}

	void CGMap::CGCtor()
	{
		Super::CGCtor();

		actorLayers_.resize(etoi(EActorLayer::Max) - 1);
		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	void CGMap::CopyFromTemplate(const CGObject& templateObj)
	{
		Super::CopyFromTemplate(templateObj);
	}

	SGReflection& CGMap::getReflection()
	{
		if (reflMap_ == false)
			reflMap_ = MakeUniqPtr<ReflectionStruct>(*this);
		else
		{	// 차후 this->actorLayers_ 에 변경이 있는 경우에만 바뀌어야한다
			reflMap_->SGRefl_Map::SGRefl_Map(*this);
		}

		return *reflMap_.get();
	}

	CGActor& CGMap::ArrangeActor(const CGActor& templateActor)
	{
		auto newActor = MakeSharPtr<CGActor>();
		newActor->CGCtor();
		newActor->CopyFromTemplate(templateActor);

		// ActorKeyGenerator
		static TActorKey ActorKeyGenerator = 0;
		newActor->actorKey_ = ++ActorKeyGenerator;

		auto rb = actorLayers_[EActorLayer::Game].emplace_back(std::move(newActor));
		return *rb;
	}

	CGActor& CGMap::OverrideActor(CGActor& arrangedActor)
	{
		// 맵에 배치된 액터에 에디터에서 편집했던 값을 적용한다
		ReflectionStruct& mapReflStruct = SCast<ReflectionStruct&>(this->getReflection());

		const auto& cont = mapReflStruct.actorLayers_[EActorLayer::Game];
		auto act = GlobalUtils::FindIt(cont, arrangedActor.actorKey_);
		if (GlobalUtils::IsFound(cont, act))
		{
			SGStringStreamIn strIn;
			strIn.in_ = *act;
			strIn >> arrangedActor.getReflection();
		}
		return arrangedActor;
	}

	void CGMap::Activate()
	{
		// actorLayers_ 로부터 액터들을 인스턴싱한다
		// ArrangeActor, OverrideActor
	}
};
