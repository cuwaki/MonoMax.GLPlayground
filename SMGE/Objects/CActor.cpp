#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
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

	SGRefl_Actor::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(TActorKey, actorKey_);
		ret += static_cast<CWString>(sg_actorTransform_);
		ret += _TO_REFL(CString, actorStaticTag_);
		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
		//ret += ReflectionUtils::ToCVector(persistentComponentsREFL_, L"CVector<SGRefl_Component>", L"persistentComponentsREFL_", std::optional<size_t>{});
		
		auto& pcomps = outerActor_.getPersistentComponents();
		ret += static_cast<CWString>(pcomps[0]->getReflection());

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

		auto& pcomps = outerActor_.getPersistentComponents();
		pcomps[0]->getReflection() = variableSplitted;

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

		// �ϴ� �� �� ���� ���
		getPersistentComponents().emplace_back(MakeUniqPtr<CMeshComponent>(this));

		auto compoUniq = getPersistentComponents().rbegin();
		cachedMainDrawCompo_ = SCast<CMeshComponent*>(compoUniq->get());

		registerComponent(compoUniq->get());
	}

	void CActor::Dtor()
	{
		getPersistentComponents().clear();
		getTransientComponents().clear();
		allComponents_.clear();

		cachedMainDrawCompo_ = nullptr;

		// ���� ���� - �̰� �ҷ��ִ� ���� ����

		Super::Dtor();
	}

	void CActor::Tick(float timeDelta)
	{
		//for(auto& comp : getAllComponents())	Tickable �� ��� �������Ѵ�
		cachedMainDrawCompo_->Tick(timeDelta);
	}

	void CActor::Render(float timeDelta)
	{
		//for(auto& comp : getAllComponents())	Renderable �� ��� �������Ѵ�
		cachedMainDrawCompo_->Render(timeDelta);
	}

	void CActor::OnAfterSpawned(CMap* map, bool isDynamic)
	{
	}

	void CActor::OnAfterArranged(CMap* map)
	{
	}

	void CActor::BeginPlay()
	{
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
