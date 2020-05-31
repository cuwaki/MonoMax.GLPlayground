#pragma once

#include "CObject.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../Interfaces/CInt_Component.h"

namespace SMGE
{
	class CActor;

	struct SGRefl_Actor : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Actor(CActor& actor);
		SGRefl_Actor(const CSharPtr<CActor>& actorPtr) : SGRefl_Actor(*actorPtr) {}

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

	class CActor : public CObject, public CInt_Reflection, public CInt_Component
	{
	public:
		using Super = CObject;
		using TReflectionStruct = SGRefl_Actor;

		friend struct TReflectionStruct;

	public:
		CActor(CObject* outer);
		CActor(CObject* outer, const CActor& templateInst);
		~CActor() noexcept;

		virtual void Ctor() override;
		virtual void Dtor() override;

		virtual void Tick(float);	// 차후 개발 - CGInterf_Tickable 로 빼자
		virtual void Render(float);	// 차후 개발 - CGInterf_Renderable 로 빼자???

		glm::mat4& getWorldTransform();
		glm::vec3& getWorldLocation();
		glm::vec3& getWorldDirection();
		glm::vec3& getWorldScale();

		void setActorStaticTag(const CString& st) { actorStaticTag_ = st; }
		CString getActorStaticTag() { return actorStaticTag_; }

		virtual void OnAfterSpawned(class CMap* map, bool isDynamic);
		virtual void OnAfterArranged(class CMap* map);
		virtual void BeginPlay();
		virtual void EndPlay();

	public:
		virtual CWString getClassName() override { return className_; }
		virtual SGReflection& getReflection() override;
		virtual ComponentVector& getComponentList() override;

	protected:
		glm::mat4 worldTransform_;
		glm::vec3 worldLocation_;
		glm::vec3 worldDirection_;
		glm::vec3 worldScale_;

		CString actorStaticTag_;
		CString actorTag_;

		class CDrawComponent* cachedMainDrawCompo_;

	protected:
		CUniqPtr<TReflectionStruct> reflActor_;
		ComponentVector components_;

		TActorKey actorKey_ = InvalidActorKey;

		// ActorKeyGenerator
		friend class CMap;
	};
};
