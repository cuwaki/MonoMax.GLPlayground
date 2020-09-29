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
		linkINST2REFL();
	}

	void SGRefl_Actor::linkINST2REFL()
	{
		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
		//persistentComponentsREFL_.resize(outerActor_.getPersistentComponents().size());
		//ReflectionUtils::clearAndEmplaceBackINST2REFL(persistentComponentsREFL_, outerActor_.getPersistentComponents());
	}

	void SGRefl_Actor::buildVariablesMap()
	{
		Super::buildVariablesMap();

		_ADD_REFL_VARIABLE(actorKey_);
		_ADD_REFL_VARIABLE(sg_actorTransform_);
		_ADD_REFL_VARIABLE(actorStaticTag_);
		// persistentComponents_ ó�� �ʿ�
	}

	void SGRefl_Actor::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();

		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
		persistentComponentNumber_ = outerActor_.getPersistentComponents().size();
	}

	SGRefl_Actor::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(TActorKey, actorKey_);
		ret += SCast<CWString>(sg_actorTransform_);
		ret += _TO_REFL(CString, actorStaticTag_);
		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
		//ret += ReflectionUtils::ToCVector(persistentComponentsREFL_, L"CVector<SGRefl_Component>", L"persistentComponentsREFL_", std::optional<size_t>{});

		// �׽�Ʈ �ڵ� �� ��Ű �ø� ���Ǿ� ����
		// �׽�Ʈ �ڵ� �� ���� �κ��̴� - CActor::Ctor ���� �����Ǿ��ִ�
		ret += _TO_REFL(int32_t, persistentComponentNumber_);

		auto& pcomps = outerActor_.getPersistentComponents();

		assert(persistentComponentNumber_ == pcomps.size());

		for (int i = 0; i < pcomps.size(); ++i)
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

		// ���� ���� - ����������ϴ� ��찡 �ְ� �ƴ� ��찡 �ִ�
		if(actorStaticTag_.length() == 0)
			_FROM_REFL(actorStaticTag_, variableSplitted);
		else
		{	// ������
			CString dummy;
			_FROM_REFL(dummy, variableSplitted);
		}

		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
			//auto FuncLoadComponent = [this](auto& variableSplitted, size_t childKey)
			//{
			//	CMeshComponent loader(nullptr);
			//	auto backupCursor = variableSplitted.cursor();
			//	loader.getReflection() = variableSplitted;

			//	auto rootAssetPath = nsGE::CGameBase::Instance->PathAssetRoot();
			//	CWString assetPath = loader.getReflection().getReflectionFilePath();
			//	if (Path::IsValidPath(assetPath) == true)
			//	{
			//		CSharPtr<CAsset<CComponent>>& componentAsset = CAssetManager::LoadAsset<CComponent>(rootAssetPath + assetPath);

			//		//variableSplitted.setCursor(backupCursor);
			//		//component->getReflection() = variableSplitted;
			//	}
			//};

			//ReflectionUtils::FromCVector(persistentComponentsREFL_, variableSplitted, FuncLoadComponent);
			//linkINST2REFL();
		// }

		// �׽�Ʈ �ڵ� �� �о���̴� �κ��̴� - ���⼭ ������ ��� �־������� CActor::Ctor ���� �����Ǿ��ִ�
		_FROM_REFL(persistentComponentNumber_, variableSplitted);
		auto& pcomps = outerActor_.getPersistentComponents();

		// �̰� ������ϴ°� �ּ¿����� 1������ Ŭ���������� 2���� �� �ִ� - �ּ¿� ������ ��ÿ��� 1�������� �ҽ��� ���ļ� 2���� ���� ���, ���� �ּ��� ���� ������� �ʾ��� �� �̷��� �� �� �ִ�
		//assert(persistentComponentNumber_ == pcomps.size());
		pcomps.clear();

		// �׽�Ʈ �ڵ� �� ����ν�� �޽������� �����Ѵ� - �̰� ����� ó���Ϸ��� RTTI �ʿ� �̽�
		//for (int i = 0; i < persistentComponentNumber_; ++i)
		//{
		//	auto meshComp = MakeUniqPtr<CMeshComponent>(&outerActor_);
		//	meshComp.get()->getReflection() = variableSplitted;
		//	pcomps.emplace_back(std::move(meshComp));
		//}

		// �׽�Ʈ �ڵ� �� ��Ű �ø� ���Ǿ� ����
		if (persistentComponentNumber_ == 2)
		{
			auto meshComp = MakeUniqPtr<CMeshComponent>(&outerActor_);
			meshComp.get()->getReflection() = variableSplitted;
			pcomps.emplace_back(std::move(meshComp));

			auto sphereComp = MakeUniqPtr<CSphereComponent>(&outerActor_);
			sphereComp.get()->getReflection() = variableSplitted;
			pcomps.emplace_back(std::move(sphereComp));
		}
		else if (persistentComponentNumber_ == 1)
		{
			auto meshComp = MakeUniqPtr<CMeshComponent>(&outerActor_);
			meshComp.get()->getReflection() = variableSplitted;
			pcomps.emplace_back(std::move(meshComp));
		}

		return *this;
	}

	SGReflection& SGRefl_Actor::operator=(CVector<CWString>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		// from fast
		_FROM_REFL(actorKey_, variableSplitted);
		//sg_actorTransform_ = variableSplitted;	// ���� deser ����������Ѵ�

		// ���� ���� - ����������ϴ� ��찡 �ְ� �ƴ� ��찡 �ִ�
		if (actorStaticTag_.length() == 0)
			_FROM_REFL(actorStaticTag_, variableSplitted);
		else
		{	// ������
			CString dummy;
			_FROM_REFL(dummy, variableSplitted);
		}

		return *this;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	DEFINE_RTTI_CObject_DEFAULT(CActor);
	DEFINE_RTTI_CObject_VARIETY(CActor, CObject*, const CActor&)

	CActor::CActor(CObject* outer) : CObject(outer)
	{
		className_ = wtext("SMGE::CActor");
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

		// ���� ���
		auto mainDrawCompo = MakeUniqPtr<CMeshComponent>(this);
		getPersistentComponents().emplace_back(std::move(mainDrawCompo));

		// �׽�Ʈ �ڵ� - ������ ���� �ڽ� ��� ������Ʈ �׽�Ʈ��
		//auto subDrawCompo = MakeUniqPtr<CMeshComponent>(this);
		////auto subDrawCompo_ = SCast<CMeshComponent*>(subDrawCompo.get());
		//getPersistentComponents().emplace_back(std::move(subDrawCompo));

		// �����Ʈ
		auto transfCompo = MakeUniqPtr<CMovementComponent>(this);
		movementCompo_ = SCast<CMovementComponent*>(transfCompo.get());
		getTransientComponents().emplace_back(std::move(transfCompo));

		// �����ص� �� - Ʈ����Ʈ �ٿ�� �׽�Ʈ �ٿ��
		auto sphereCompo = MakeUniqPtr<CPointComponent>(this);
		getTransientComponents().emplace_back(std::move(sphereCompo));
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
		// �׽�Ʈ �ڵ� �� ��Ű �ø� ���Ǿ� ����
		if(getActorStaticTag() == "this is a monkey")
			mainBoundCompo_ = DCast<CBoundComponent*>(getPersistentComponents()[1].get());
		
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
		className_ = wtext("SMGE::CCollideActor");

		rule_ = rule;
		isDetailCheck_ = isDetailCheck;
	}

	void CCollideActor::BeginPlay()
	{
		Super::BeginPlay();
	}

	DEFINE_RTTI_CObject_DEFAULT(CPointActor);
	
	CPointActor::CPointActor(CObject* outer) : CActor(outer)
	{
		className_ = wtext("SMGE::CPointActor");
		Ctor();
	}

	void CPointActor::Ctor()
	{
		//Super::Ctor();	// �Ϻη� - �� Ŭ�������� �����ڿ��� ���� �ҷ��ֹǷ� �̷��� �θ��� �ȵȴ�! ����� �׷��� 20200831

		// ���� ������ �� - ���� CActor ����̶� �Ⱦ��� persistcompo - mainDrawCompo ���� ����ٰ� ��������, �� ������ �𸮾󿡼��� �־���
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();
		movementCompo_ = nullptr;
		mainBoundCompo_ = nullptr;

		auto pointCompo = MakeUniqPtr<CPointComponent>(this);
		getTransientComponents().emplace_back(std::move(pointCompo));
	}

	DEFINE_RTTI_CObject_DEFAULT(CRayCollideActor);
	DEFINE_RTTI_CObject_VARIETY(CRayCollideActor, CObject*, ECheckCollideRule, bool, const DELEGATE_OnCollide&, float, const glm::vec3&);

	CRayCollideActor::CRayCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, float size, const glm::vec3& dir) :
		CCollideActor(outer, rule, isDetailCheck, fOnCollide)
	{
		className_ = wtext("SMGE::CRayCollideActor");
		Ctor(size, dir);
	}

	void CRayCollideActor::Ctor(float size, const glm::vec3& dir)
	{
		//Super::Ctor();	// �Ϻη� - �� Ŭ�������� �����ڿ��� ���� �ҷ��ֹǷ� �̷��� �θ��� �ȵȴ�! ����� �׷��� 20200831

		// ���� ������ �� - ���� CActor ����̶� �Ⱦ��� persistcompo - mainDrawCompo ���� ����ٰ� ��������, �� ������ �𸮾󿡼��� �־���
		getPersistentComponents().clear();
		getTransientComponents().clear();
		getAllComponents().clear();
		movementCompo_ = nullptr;
		mainBoundCompo_ = nullptr;

		// �ٿ��
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
					// ������ �ٿ�� ������ ���Ͽ� üũ - �������� �߰� Bound �Ǵ� �������� CPolygonComponent �� �־�߰���?
				}
			}
		}
	}
};
