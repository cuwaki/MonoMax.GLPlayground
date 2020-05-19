#include "CGActor.h"
#include "../Components/CGStaticMeshComponent.h"

namespace MonoMaxGraphics
{
	SGRefl_Actor::SGRefl_Actor(CGActor& actor) :
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

	CGActor::CGActor() : CGObject()
	{
		className_ = wtext("SMGE_Game::CGActor");
		CGCtor();
	}

	CGActor::CGActor(const CGActor& templateInst) : CGActor()
	{
		CopyFromTemplate(templateInst);
	}

	SGReflection& CGActor::getReflection()
	{
		if (reflActor_.get() == nullptr)
			reflActor_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflActor_.get();
	}

	ComponentVector& CGActor::getComponentList()
	{
		return components_;
	}

	void CGActor::CGCtor()
	{
		Super::CGCtor();

		getComponentList().emplace_back(MakeUniqPtr<CGStaticMeshComponent>());
		cachedMainDrawCompo_ = SCast<CGStaticMeshComponent*>(getComponentList().rbegin()->get());
	}

	glm::mat4& CGActor::getWorldTransform()
	{
		return worldTransform_;
	}
	glm::vec3& CGActor::getWorldLocation()
	{
		return worldLocation_;
	}
	glm::vec3& CGActor::getWorldDirection()
	{
		return worldDirection_;
	}
	glm::vec3& CGActor::getWorldScale()
	{
		return worldScale_;
	}
};
