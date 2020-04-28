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
		{	// this->actorLayers_ �� ������ �ִ� ��� �ٲ����Ѵ�
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

			�̷��� �ؼ� �⺻���� ���� ���͸� �ʿ� ��ġ�Ѵ�

			��ӵ� ���͸� ��ġ�� �� �־��Ѵ� - reflClassName_ Ȱ��
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
			�ʿ� ��ġ�� ���Ϳ� �����Ϳ��� �����ߴ� ���� �����Ѵ�
		*/
	}
};
