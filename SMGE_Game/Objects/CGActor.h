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

		bool operator==(const SGRefl_Actor& right) const noexcept
		{
			return this->actorKey_ == right.actorKey_;
		}
		bool operator==(const TActorKey& ak) const noexcept
		{
			return this->actorKey_ == ak;
		}

		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value> &in) override;
		virtual SGReflection& operator=(CVector<CWString>& in) override;

		TActorKey& actorKey_;
		glm::mat4& worldTransform_;
		CString& actorStaticTag_;
	};

	class CGActor : public CGObject,
		public CGInterf_Reflection, public CGInterf_Component
	{
	public:
		using Super = CGObject;
		using TReflectionStruct = SGRefl_Actor;

		friend struct TReflectionStruct;

	public:
		CGActor();
		CGActor(const CGActor& templateInst);

		virtual void CGCtor() override;

		glm::mat4& getWorldTransform();
		glm::vec3& getWorldLocation();
		glm::vec3& getWorldDirection();
		glm::vec3& getWorldScale();

		void setActorStaticTag(const CString& st) { actorStaticTag_ = st; }
		CString getActorStaticTag() { return actorStaticTag_; }

	public:
		virtual SGReflection& getReflection() override;
		virtual ComponentVector& getComponentList() override;

	protected:
		glm::mat4 worldTransform_;
		glm::vec3 worldLocation_;
		glm::vec3 worldDirection_;
		glm::vec3 worldScale_;

		CString actorStaticTag_;
		CString actorTag_;

		class CGDrawComponent* cachedMainDrawCompo_;

	protected:
		CUniqPtr<TReflectionStruct> reflActor_;
		ComponentVector components_;

		TActorKey actorKey_ = InvalidActorKey;

		// ActorKeyGenerator
		friend class CGMap;
	};
};
