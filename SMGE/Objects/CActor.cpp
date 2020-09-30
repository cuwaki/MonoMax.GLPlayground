#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
#include "../Components/CMovementComponent.h"
#include "../Components/CSphereComponent.h"
#include "../Components/CRayComponent.h"
#include "../Components/CPointComponent.h"
#include "../Assets/CAssetManager.h"
#include "../CGameBase.h"

#include <algorithm>

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Actor::SGRefl_Actor(CActor& actor) :
		actorKey_(actor.actorKey_),
		sg_actorTransform_(actor, actor.actorTransform_),
		actorStaticTag_(actor.actorStaticTag_),	
		outerActor_(actor), SGReflection(actor)
	{
	}

	void SGRefl_Actor::buildVariablesMap()
	{
		Super::buildVariablesMap();

		_ADD_REFL_VARIABLE(actorKey_);
		_ADD_REFL_VARIABLE(sg_actorTransform_);
		_ADD_REFL_VARIABLE(actorStaticTag_);
		// persistentComponents_ 처리 필요
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
		//classRTTIName_ = "SMGE::CActor";
		Ctor();
	}

	CActor::CActor(CObject* outer, const CActor& templateInst) : CActor(outer)
	{
		CopyFromTemplate(templateInst);
	}

	CActor::~CActor()
	{
		Dtor();
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

	void CActor::Ctor()
	{
		Super::Ctor();

		// 무브먼트
		auto transfCompo = MakeUniqPtr<CMovementComponent>(this);
		movementCompo_ = SCast<CMovementComponent*>(transfCompo.get());
		getTransientComponents().emplace_back(std::move(transfCompo));
	}

	void CActor::Dtor()
	{
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();

		movementCompo_ = nullptr;
		mainBoundCompo_ = nullptr;

		Super::Dtor();
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

		mainBoundCompo_ = findComponent<CBoundComponent>();	// 처음 만나는 바운드를 메인 바운드로 삼는다

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

		for (auto& pc : getPersistentComponents())
		{
			unregisterComponent(pc.get());
		}

		for (auto& pc : getTransientComponents())
		{
			unregisterComponent(pc.get());
		}
	}

	class CBoundComponent* CActor::GetMainBound()
	{
		return mainBoundCompo_;
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

	CCollideActor::CCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide) : CActor(outer), fOnCollide_(fOnCollide)
	{
		//classRTTIName_ = "SMGE::CCollideActor";

		rule_ = rule;
		isDetailCheck_ = isDetailCheck;
	}

	void CCollideActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	CPointActor::CPointActor(CObject* outer) : CCollideActor(outer)
	{
		//classRTTIName_ = "SMGE::CPointActor";
		Ctor();
	}

	void CPointActor::Ctor()
	{
		//Super::Ctor();	// 일부러 - 각 클래스들의 생성자에서 따로 불러주므로 이렇게 부르면 안된다! 현재는 그렇다 20200831

		// 차후 생각할 일 - 현재 CActor 상속이라서 안쓰는 persistcompo - mainDrawCompo 등이 생겼다가 없어진다, 이 문제는 언리얼에서도 있었다
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();
		movementCompo_ = nullptr;
		mainBoundCompo_ = nullptr;

		auto pointCompo = MakeUniqPtr<CPointComponent>(this);
		getTransientComponents().emplace_back(std::move(pointCompo));
	}

	CRayCollideActor::CRayCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, float size, const glm::vec3& dir) :
		CCollideActor(outer, rule, isDetailCheck, fOnCollide)
	{
		//classRTTIName_ = "SMGE::CRayCollideActor";
		Ctor(size, dir);
	}

	void CRayCollideActor::Ctor(float size, const glm::vec3& dir)
	{
		//Super::Ctor();	// 일부러 - 각 클래스들의 생성자에서 따로 불러주므로 이렇게 부르면 안된다! 현재는 그렇다 20200831

		// 차후 생각할 일 - 현재 CActor 상속이라서 안쓰는 persistcompo - mainDrawCompo 등이 생겼다가 없어진다, 이 문제는 언리얼에서도 있었다
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();
		movementCompo_ = nullptr;
		mainBoundCompo_ = nullptr;

		// 바운드
		auto RayCompo = MakeUniqPtr<CRayComponent>(this, size, dir);
		mainBoundCompo_ = SCast<CRayComponent*>(RayCompo.get());
		getTransientComponents().emplace_back(std::move(RayCompo));
	}

	std::vector<CActor*> CRayCollideActor::QueryCollideCheckTargets()
	{
		const auto& actors = Globals::GCurrentMap->GetActors(EActorLayer::Game);

		std::vector<CActor*> ret(actors.size());
		std::transform(actors.begin(), actors.end(), ret.begin(), [this](const CSharPtr<CActor>& sptrActor)
			{
				auto ret = sptrActor.get();
				if (ret != nullptr && ret->GetMainBound() != nullptr && ret != this)
					return ret;
				else
					ret = nullptr;

				return ret;
			});

		auto nullStart = std::remove_if(ret.begin(), ret.end(), [](auto& v) { return v == nullptr; });
		ret.erase(nullStart, ret.end());

		return ret;
	}

	void CRayCollideActor::ProcessCollide(std::vector<CActor*>& targets)
	{
		ProcessCollide(rule_, isDetailCheck_, fOnCollide_, targets);
	}

	void CRayCollideActor::ProcessCollide(ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, std::vector<CActor*>& targets)
	{
		glm::vec3 collidingPoint;

		for (auto& actor : targets)
		{
			if (this->GetMainBound()->CheckCollide(actor->GetMainBound(), collidingPoint) == true)
			{
				if (isDetailCheck == false)
				{
					fOnCollide(this, actor, this->GetMainBound(), actor->GetMainBound(), collidingPoint);
				}
				else
				{
					// 디테일 바운드 콤포에 대하여 체크 - 여러개의 추가 Bound 또는 디테일한 CPolygonComponent 가 있어야겠지?
				}
			}
		}
	}

	std::vector<CActor*> CPointActor::QueryCollideCheckTargets()
	{
		return std::vector<CActor*>();
	}

	void CPointActor::ProcessCollide(ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, std::vector<CActor*>& targets)
	{
	}
};
