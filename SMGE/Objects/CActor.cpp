#include "CActor.h"
#include "CMap.h"
#include "../Components/CMeshComponent.h"
#include "../Assets/CAssetManager.h"

namespace SMGE
{
	SGRefl_Actor::SGRefl_Actor(CActor& actor) :
		actorKey_(actor.actorKey_),
		worldTransform_(actor.worldTransform_),
		actorStaticTag_(actor.actorStaticTag_),
		SGReflection(actor)
	{
	}

	void SGRefl_Actor::buildVariablesMap()
	{
		Super::buildVariablesMap();

		_ADD_REFL_VARIABLE(actorKey_);
		_ADD_REFL_VARIABLE(worldTransform_);
		_ADD_REFL_VARIABLE(actorStaticTag_);
	}

	SGRefl_Actor::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(TActorKey, actorKey_);
		ret += _TO_REFL(glm::mat4, worldTransform_);
		ret += _TO_REFL(CString, actorStaticTag_);
		
		return ret;
	}

	SGReflection& SGRefl_Actor::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(actorKey_, variableSplitted);
		_FROM_REFL(worldTransform_, variableSplitted);
		_FROM_REFL(actorStaticTag_, variableSplitted);

		return *this;
	}

	SGReflection& SGRefl_Actor::operator=(CVector<CWString>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		// from fast
		_FROM_REFL(actorKey_, variableSplitted);
		_FROM_REFL(worldTransform_, variableSplitted);
		_FROM_REFL(actorStaticTag_, variableSplitted);

		return *this;
	}

	CActor::CActor(CObject* outer) : CObject(outer)
	{
		className_ = wtext("SMGE::CActor");
		worldTransform_ = glm::mat4(1.f);
		worldLocation_ = glm::vec3(0.f);
		worldDirection_ = glm::vec3(1.f, 0.f, 0.f);
		worldScale_ = glm::vec3(1.f, 1.f, 1.f);
		Ctor();
	}

	CActor::CActor(CObject* outer, const CActor& templateInst) : CActor(outer)
	{
		CopyFromTemplate(templateInst);
	}

	CActor::~CActor()
	{
	}

	SGReflection& CActor::getReflection()
	{
		if (reflActor_.get() == nullptr)
			reflActor_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflActor_.get();
	}

	ComponentVector& CActor::getComponentList()
	{
		return components_;
	}

	void CActor::Ctor()
	{
		Super::Ctor();

		auto testModelAsset = CAssetManager::FindAssetFilePathByClassName(wtext("SMGE::CAssetModel"));	// 테스트 코드
		getComponentList().emplace_back(MakeUniqPtr<CMeshComponent>(this, testModelAsset));
		cachedMainDrawCompo_ = SCast<CMeshComponent*>(getComponentList().rbegin()->get());
	}

	void CActor::Dtor()
	{
		getComponentList().clear();
		cachedMainDrawCompo_ = nullptr;

		Super::Dtor();
	}

	void CActor::Tick(float timeDelta)
	{
		//for(auto& comp : getComponentList())	Tickable 만 골라서 돌려야한다
		cachedMainDrawCompo_->Tick(timeDelta);
	}

	void CActor::Render(float timeDelta)
	{
		//for(auto& comp : getComponentList())	Renderable 만 골라서 돌려야한다
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
		for (auto& comp : getComponentList())
		{
			comp->OnBeginPlay(this);
		}
	}

	void CActor::EndPlay()
	{
		for (auto& comp : getComponentList())
		{
			comp->OnEndPlay();
		}
	}

	glm::mat4& CActor::getWorldTransform()
	{
		return worldTransform_;
	}
	glm::vec3& CActor::getWorldLocation()
	{
		return worldLocation_;
	}
	glm::vec3& CActor::getWorldDirection()
	{
		return worldDirection_;
	}
	glm::vec3& CActor::getWorldScale()
	{
		return worldScale_;
	}
};
