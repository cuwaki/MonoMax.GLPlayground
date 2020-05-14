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
		if (reflMap_.get() == nullptr)
			reflMap_ = MakeUniqPtr<ReflectionStruct>(*this);
		else
		{	// ���� this->actorLayers_ �� ������ �ִ� ��쿡�� �ٲ����Ѵ�
			reflMap_->SGRefl_Map::SGRefl_Map(*this);
		}

		return *reflMap_.get();
	}

	SGReflection& CGMap::getReflection2()
	{
		if (reflMap_.get() == nullptr)
			reflMap_ = MakeUniqPtr<ReflectionStruct>(*this);
		else
		{	// ���� this->actorLayers_ �� ������ �ִ� ��쿡�� �ٲ����Ѵ�
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
		// �ʿ� ��ġ�� ���Ϳ� �����Ϳ��� �����ߴ� ���� �����Ѵ�
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
		// actorLayers_ �κ��� ���͵��� �ν��Ͻ��Ѵ�
		// ArrangeActor, OverrideActor
	}
};
