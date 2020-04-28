#pragma once

#include "CGObject.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "../Interfaces/CGInterf_Component.h"

namespace MonoMaxGraphics
{
	class CGActor;

	struct SGRefl_Actor : public SGReflection
	{
		SGRefl_Actor(CGActor& actor);
		SGRefl_Actor(const CSharPtr<CGActor>& actorPtr);

		ActorKey& actorKey_;
		glm::mat4& worldTransform_;
	};

	class CGActor : public CGObject,
		public CGInterf_Reflection, public CGInterf_Component
	{
		using Super = CGObject;
		using ReflectionStruct = SGRefl_Actor;

		friend struct ReflectionStruct;

	public:
		CGActor();

		virtual void MakeDefault() override;

		glm::mat4& getWorldTransform();
		glm::vec3& getWorldLocation();
		glm::vec3& getWorldDirection();
		glm::vec3& getWorldScale();

	public:
		virtual SGReflection& getReflection() override;
		virtual ComponentVector& getComponentList() override;

	protected:
		glm::mat4 worldTransform_;
		glm::vec3 worldLocation_;
		glm::vec3 worldDirection_;
		glm::vec3 worldScale_;

		class CGDrawComponent* cachedMainDrawCompo_;

	protected:
		CUniqPtr<ReflectionStruct> reflActor_;
		ComponentVector components_;

		ActorKey actorKey_ = InvalidActorKey;
	};
};
