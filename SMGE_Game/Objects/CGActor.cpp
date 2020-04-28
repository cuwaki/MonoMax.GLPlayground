#include "CGActor.h"
#include "../Components/CGStaticMeshComponent.h"

namespace MonoMaxGraphics
{
	SGRefl_Actor::SGRefl_Actor(CGActor& actor) :
		worldTransform_(actor.worldTransform_), actorKey_(actor.actorKey_),
		SGReflection(actor)
	{
	}

	SGRefl_Actor::SGRefl_Actor(const CSharPtr<CGActor>& actorPtr) :
		worldTransform_(actorPtr->worldTransform_), actorKey_(actorPtr->actorKey_),
		SGReflection(*actorPtr)
	{
	}

	CGActor::CGActor()
	{
	}

	SGReflection& CGActor::getReflection()
	{
		if (reflActor_ == false)
			reflActor_ = MakeUniqPtr<ReflectionStruct>(*this);
		return *reflActor_.get();
	}

	ComponentVector& CGActor::getComponentList()
	{
		return components_;
	}

	void CGActor::MakeDefault()
	{
		Super::MakeDefault();

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
