#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
#include "../Components/CMovementComponent.h"
#include "../Components/CSphereComponent.h"
#include "../Assets/CAssetManager.h"
#include "../CGameBase.h"

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

		for (int i = 0; i < persistentComponentNumber_; ++i)
		{
			// �׽�Ʈ �ڵ� �� ����ν�� �޽������� �����Ѵ� - �̰� ����� ó���Ϸ��� RTTI �ʿ� �̽�
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

		// �ٿ��
		auto sphereCompo = MakeUniqPtr<CSphereComponent>(this);
		mainBoundCompo_ = SCast<CSphereComponent*>(sphereCompo.get());
		getTransientComponents().emplace_back(std::move(sphereCompo));
	}

	void CActor::Dtor()
	{
		getPersistentComponents().clear();
		getTransientComponents().clear();
		allComponents_.clear();

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

	void CActor::OnAfterSpawned(CMap* map, bool isDynamic)
	{
	}

	void CActor::OnAfterArranged(CMap* map)
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

		for (auto& pc : getPersistentComponents())
		{
			unregisterComponent(pc.get());
		}

		for (auto& pc : getTransientComponents())
		{
			unregisterComponent(pc.get());
		}
	}

	nsRE::Transform& CActor::getTransform()
	{
		return actorTransform_;
	}

	const nsRE::Transform& CActor::getTransform() const
	{
		return actorTransform_;
	}
};
