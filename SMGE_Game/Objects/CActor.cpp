#include "CActor.h"
#include "CMap.h"
#include "Components/CMeshComponent.h"
#include "Assets/CAssetManager.h"

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

	CActor::CActor() : CObject()
	{
		className_ = wtext("SMGE_Game::CActor");
		CGCtor();
	}

	CActor::CActor(const CActor& templateInst) : CActor()
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

	void CActor::CGCtor()
	{
		Super::CGCtor();

		getComponentList().emplace_back(MakeUniqPtr<CMeshComponent>());
		cachedMainDrawCompo_ = SCast<CMeshComponent*>(getComponentList().rbegin()->get());
	}

	void CActor::Tick(float timeDelta)
	{
		cachedMainDrawCompo_->Tick(timeDelta);
	}

	void CActor::Render(float timeDelta)
	{
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
		CWString modelAssetPath = CAssetManager::FindAssetFilePathByClassName(wtext("SMGE_Game::CModelData"));	// 테스트 코드
		cachedMainDrawCompo_->ReadyToDrawing(modelAssetPath);
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
