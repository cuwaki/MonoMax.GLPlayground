#pragma once

#include "CGObject.h"
#include "../Interfaces/CGInterf_Reflection.h"
#include "../Interfaces/CGInterf_Component.h"

namespace MonoMaxGraphics
{
	class CGActor;

	struct SGRefl_Actor : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Actor(CGActor& actor);
		SGRefl_Actor(const CSharPtr<CGActor>& actorPtr) : SGRefl_Actor(*actorPtr) {}

		virtual void buildVariablesMap() override;

		virtual operator CWString() const override;

	protected:
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value> &in) override;
		virtual SGReflection& operator=(CVector<CWString>& in) override;

		TActorKey& actorKey_;
		glm::mat4& worldTransform_;
	};

	class CGActor : public CGObject,
		public CGInterf_Reflection, public CGInterf_Component
	{
	public:
		using Super = CGObject;
		using ReflectionStruct = SGRefl_Actor;

		friend struct ReflectionStruct;

	public:
		CGActor();

		virtual void CGCtor() override;
		virtual void CopyFromTemplate(const CGObject& templateObj) override;

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

		TActorKey actorKey_ = InvalidActorKey;

		// ActorKeyGenerator
		friend class CGMap;
	};
};
