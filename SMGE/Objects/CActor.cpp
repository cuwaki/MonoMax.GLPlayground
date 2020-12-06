#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
#include "../Components/CMovementComponent.h"
#include "../Components/CSphereComponent.h"
#include "../Components/CSegmentComponent.h"
#include "../Components/CPointComponent.h"
#include "../Assets/CAssetManager.h"
#include "../CGameBase.h"

#include "../Components/CCubeComponent.h"

#include <algorithm>

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Actor::SGRefl_Actor(CActor& actor) :
		actorKey_(actor.actorKey_),
		sg_actorTransform_(actor, actor.actorTransform_),
		actorStaticTag_(actor.actorStaticTag_),	
		outerActor_(actor),
		SGReflection(actor)
	{
	}

	SGRefl_Actor::SGRefl_Actor(const CSharPtr<CActor>& actorPtr) : SGRefl_Actor(*actorPtr)
	{
		classRTTIName_ = actorPtr->getReflection().getClassRTTIName();	// 이미 로드된 액터로부터 사본으로 생기는 경우라서 이걸 수동으로 복사해줘야한다
		reflectionFilePath_ = actorPtr->getReflection().getReflectionFilePath();
	}

	void SGRefl_Actor::buildVariablesMap()
	{
		Super::buildVariablesMap();

		_ADD_REFL_VARIABLE(actorKey_);
		_ADD_REFL_VARIABLE(sg_actorTransform_);
		_ADD_REFL_VARIABLE(actorStaticTag_);
	}

	void SGRefl_Actor::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_Actor::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(TActorKey, actorKey_);
		ret += SCast<CWString>(sg_actorTransform_);
		ret += _TO_REFL(CString, actorStaticTag_);

		auto persistCompoSize = outerActor_.getPersistentComponents().size();
		ret += _TO_REFL(size_t, persistCompoSize);

		auto& pcomps = outerActor_.getPersistentComponents();
		for (size_t i = 0; i < pcomps.size(); ++i)
		{
			ret += SCast<CWString>(pcomps[i]->getReflection());
		}

		return ret;
	}

	SGReflection& SGRefl_Actor::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(actorKey_, variableSplitted);
		sg_actorTransform_ = variableSplitted;

		// 여기 수정 - 덮어씌워져야하는 경우가 있고 아닌 경우가 있다
		if(actorStaticTag_.length() == 0)
			_FROM_REFL(actorStaticTag_, variableSplitted);
		else
		{	// 버린다
			CString dummy;
			_FROM_REFL(dummy, variableSplitted);
		}

		size_t persistCompoSize = 0;
		_FROM_REFL(persistCompoSize, variableSplitted);

		auto& pcomps = outerActor_.getPersistentComponents();
		pcomps.clear();
		pcomps.reserve(persistCompoSize);

		for (size_t i = 0; i < persistCompoSize; ++i)
		{
			auto compo = ReflectionUtils::FuncLoadClass<CComponent>(&outerActor_, variableSplitted);
			if(compo != nullptr)
				pcomps.emplace_back(std::move(compo));
		}

		return *this;
	}

	SGReflection& SGRefl_Actor::operator=(CVector<CWString>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		// from fast
		_FROM_REFL(actorKey_, variableSplitted);
		//sg_actorTransform_ = variableSplitted;	// 빠른 deser 지원해줘야한다

		// 여기 수정 - 덮어씌워져야하는 경우가 있고 아닌 경우가 있다
		if (actorStaticTag_.length() == 0)
			_FROM_REFL(actorStaticTag_, variableSplitted);
		else
		{	// 버린다
			CString dummy;
			_FROM_REFL(dummy, variableSplitted);
		}

		return *this;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CActor::CActor(CObject* outer) : CObject(outer)
	{
		mainBoundCompo_ = nullptr;

		Ctor();
	}

	CActor::~CActor()
	{
		Dtor();
	}

	void CActor::Ctor()
	{
	}

	void CActor::Dtor()
	{
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();

		Super::Dtor();
	}

	SGReflection& CActor::getReflection()
	{
		if (reflActor_.get() == nullptr)
			reflActor_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflActor_.get();
	}

	void CActor::OnAfterDeserialized()
	{
	}

	ComponentVector& CActor::getPersistentComponents()
	{
		return persistentComponents_;
	}
	ComponentVector& CActor::getTransientComponents()
	{
		return transientComponents_;
	}
	ComponentVectorWeak& CActor::getAllComponents()
	{
		return allComponents_;
	}

	void CActor::Tick(float timeDelta)
	{
		for (auto& comp : getAllComponents())
		{
			comp->Tick(timeDelta);
		}

		if (lifeTick_ > 0 && --lifeTick_ == 0)
		{
			SetPendingKill();
		}
	}

	void CActor::Render(float timeDelta)
	{
		for (auto& comp : getAllComponents())
		{
			auto mc = DCast<CDrawComponent*>(comp);
			if(mc)
				mc->Render(timeDelta);
		}
	}

	void CActor::OnSpawnStarted(CMap* map, bool isDynamic)
	{
	}

	void CActor::OnSpawnFinished(CMap* map)
	{
	}

	void CActor::BeginPlay()
	{
		for (auto& pc : getPersistentComponents())
		{
			registerComponent(pc.get());
		}
		for (auto& pc : getTransientComponents())
		{
			registerComponent(pc.get());
		}

		timer_.start();

		for (auto& comp : getAllComponents())
		{
			comp->OnBeginPlay(this);
		}
	}

	void CActor::EndPlay()
	{
		for (auto& comp : getAllComponents())
		{
			comp->OnEndPlay();
		}

		unregisterComponentAll();
	}

	nsRE::Transform& CActor::getTransform()
	{
		return actorTransform_;
	}
	const nsRE::Transform& CActor::getTransform() const
	{
		return actorTransform_;
	}

	void CActor::SetPendingKill()
	{
		isPendingKill_ = true;
	}
	bool CActor::IsPendingKill() const
	{
		return isPendingKill_;
	}

	class CBoundComponent* CActor::GetMainBound()
	{
		if (mainBoundCompo_ == nullptr)
			mainBoundCompo_ = findComponent<CBoundComponent>();	// 처음 만나는 바운드를 메인 바운드로 삼는다

		return mainBoundCompo_;
	}
};
