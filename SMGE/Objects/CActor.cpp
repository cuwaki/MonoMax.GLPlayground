#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
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

	SGRefl_Actor::SGRefl_Actor(const std::unique_ptr<CActor>& actorPtr) : SGRefl_Actor(*actorPtr)
	{
		classRTTIName_ = actorPtr->getReflection().getClassRTTIName();	// 이미 로드된 액터로부터 사본으로 생기는 경우라서 이걸 수동으로 복사해줘야한다
		reflectionFilePath_ = actorPtr->getReflection().getSourceFilePath();
	}

	void SGRefl_Actor::buildVariablesMap()
	{
		Super::buildVariablesMap();

		_ADD_REFL_VARIABLE(actorKey_);
		_ADD_REFL_VARIABLE(sg_actorTransform_);
		_ADD_REFL_VARIABLE(actorStaticTag_);
	}

	const SGReflection& SGRefl_Actor::operator>>(CWString& out) const
	{
		Super::operator>>(out);

		out += _TO_REFL(TActorKey, actorKey_);
		sg_actorTransform_ >> out;
		out += _TO_REFL(CString, actorStaticTag_);

		auto persistCompoSize = outerActor_.getPersistentComponents().size();
		out += _TO_REFL(size_t, persistCompoSize);

		auto& pcomps = outerActor_.getPersistentComponents();
		for (size_t i = 0; i < pcomps.size(); ++i)
		{
			pcomps[i]->getReflection() >> out;
		}

		return *this;
	}

	SGReflection& SGRefl_Actor::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		_FROM_REFL(actorKey_, in);
		sg_actorTransform_ << in;

		_FROM_REFL(actorStaticTag_, in);

		size_t persistCompoSize = 0;
		_FROM_REFL(persistCompoSize, in);

		auto& pcomps = outerActor_.getPersistentComponents();
		pcomps.clear();
		pcomps.reserve(persistCompoSize);

		for (size_t i = 0; i < persistCompoSize; ++i)
		{
			auto compo = ReflectionUtils::FuncLoadClass<CComponent>(&outerActor_, in);
			if(compo != nullptr)
				pcomps.emplace_back(std::move(compo));
		}

		return *this;
	}

	SGReflection& SGRefl_Actor::operator<<(const CVector<CWString>& in)
	{
		Super::operator<<(in);

		// from fast
		_FROM_REFL(actorKey_, in);
		//sg_actorTransform_ = in;	// 빠른 deser 지원해줘야한다

		_FROM_REFL(actorStaticTag_, in);

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
			reflActor_ = std::make_unique<TReflectionStruct>(*this);
		return *reflActor_.get();
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

		if (lifeTickCount_ > 0 && --lifeTickCount_ == 0)
		{
			SetPendingKill();
		}
	}

	void CActor::AfterTick(float timeDelta)
	{
		for (auto& comp : getAllComponents())
		{
			comp->AfterTick(timeDelta);
		}

		if (IsPendingKill() == false)
		{
			getTransform().RecalcFinal();	// 현재 눈에 보이는 콤포넌트가 하나도 없는 경우가 있으므로 여기서 해줘야한다, 여기서 해주면 나중에 안하니까 문제 없음
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

		getTransform().RecalcFinal();	// 이제 부모 자식 관계가 설정되었으므로 Final 계산을 제대로 할 수 있다.
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

	void CActor::SetRendering(bool isr, bool propagate)
	{
		if (propagate == false && isRendering_ == isr)
			return;

		auto hasDrawCompo = false;

		// 액터에서 바로 밑 콤포넌트들에게는 무조건 해야한다 - 액터가 WorldModel 가 아니기 때문
		for (auto comp : getAllComponents())
		{
			auto drawComp = dynamic_cast<CDrawComponent*>(comp);
			if (drawComp)
			{
				drawComp->SetRendering(isr, propagate);
				hasDrawCompo = true;
			}
		}

		isRendering_ = hasDrawCompo;	// 드로콤포가 하나도 없다면 플래그를 꺼야 CRenderingPass 에서 빠른 처리가 가능하다
	}

	class CBoundComponent* CActor::GetMainBound()
	{
		// 여기 - 최적화 - 콤포넌트의 변화가 있을 때만 재계산이 되어야한다
		if (mainBoundCompo_ == nullptr)
		{	// 여기 - transient 를 메인으로 삼은 경우 문제가 될 수 있는 점이 개선되어야한다
			mainBoundCompo_ = findComponent<CBoundComponent>([](auto compoPtr)
				{
					auto bc = dynamic_cast<CBoundComponent*>(compoPtr);
					if (bc && bc->IsCollideTarget())
						return true;
					return false;
				});
		}
		
		return mainBoundCompo_;
	}
};
