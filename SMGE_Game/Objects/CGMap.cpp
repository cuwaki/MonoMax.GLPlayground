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

	void CGMap::MakeDefault()
	{
		Super::MakeDefault();

		actorLayers_.resize(etoi(EActorLayer::Max) - 1);

		actorLayers_[EActorLayer::System].reserve(20);
		actorLayers_[EActorLayer::Game].reserve(100);
	}

	SGReflection& CGMap::getReflection()
	{
		if (reflMap_ == false)
			reflMap_ = MakeUniqPtr<ReflectionStruct>(*this);
		else
		{	// this->actorLayers_ 에 변경이 있는 경우 바뀌어야한다
			clearAndAssign(reflMap_->actorLayers_[EActorLayer::System], this->actorLayers_[EActorLayer::System]);
			clearAndAssign(reflMap_->actorLayers_[EActorLayer::Game], this->actorLayers_[EActorLayer::Game]);
		}

		return *reflMap_.get();
	}

	void CGMap::ArrangeActor(const CGActor& templateActor)
	{
		/**
			new actor class
			MakeDefault
			LoadFrom Actor Template Asset

			이렇게 해서 기본값을 가진 액터를 맵에 배치한다

			상속된 액터를 배치할 수 있어한다 - reflClassName_ 활용
		*/
		//auto newActor = MakeSharPtr<CGActor>(templateActor);
		//newActor->MakeDefault();
		////newActor->getReflection() = 

		//auto added = actorLayers_[EActorLayer::Game].emplace_back();
		//added->MakeDefault();
	}

	void CGMap::OverrideActor(CGActor& arrangedActor)
	{
		/**
			맵에 배치된 액터에 에디터에서 편집했던 값을 적용한다
		*/
	}
};
