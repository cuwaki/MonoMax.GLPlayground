#pragma once

#include "CObject.h"
#include "../CActorInterface.h"
#include "../Interfaces/CInt_Reflection.h"
#include "../Interfaces/CInt_Component.h"
#include "../Components/CDrawComponent.h"
#include "../Components/CBoundComponent.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"
#include "../CTimer.h"
#include <functional>

namespace SMGE
{
	class CActor;

	struct SGRefl_Actor : public SGReflection
	{
		using Super = SGReflection;

		SGRefl_Actor(CActor& actor);
		SGRefl_Actor(const CUniqPtr<CActor>& actorPtr);
		
		virtual void buildVariablesMap() override;
		
		virtual void OnBeforeSerialize() const override;

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value> &in) override;

		virtual SGReflection& operator=(CVector<CWString>& in) override;

		bool operator==(const SGRefl_Actor& right) const noexcept
		{
			return this->actorKey_ == right.actorKey_;
		}
		bool operator==(const TActorKey& ak) const noexcept
		{
			return this->actorKey_ == ak;
		}

		TActorKey& actorKey_;
		SGRefl_Transform sg_actorTransform_;
		CString& actorStaticTag_;

		// for Runtime
		CActor& outerActor_;
	};

	class CActor : public CObject, public CInt_Reflection, public CInt_Component, public CActorInterface
	{
		DECLARE_RTTI_CObject(CActor)

	public:
		using This = CActor;
		using Super = CObject;
		using TReflectionStruct = SGRefl_Actor;

		friend struct TReflectionStruct;

	public:
		CActor(CObject* outer);
		~CActor() noexcept;

		void Ctor();
		void Dtor();

		virtual void Tick(float);	// 차후 개발 - CGInterf_Tickable 로 빼자

		nsRE::Transform& getTransform();
		const nsRE::Transform& getTransform() const;

		void setActorStaticTag(const CString& st) { actorStaticTag_ = st; }
		CString getActorStaticTag() { return actorStaticTag_; }

		virtual void OnSpawnStarted(class CMap* map, bool isDynamic);
		virtual void OnSpawnFinished(class CMap* map);
		virtual void BeginPlay();
		virtual void EndPlay();

		CTimer& getActorTimer() { return timer_; }

		const glm::vec3& getLocation() const {	return getTransform().GetTranslation(); }
		const glm::vec3& getRotationEuler() const { return getTransform().GetRotationEuler(); }
		glm::vec3 getRotationEulerDegrees() const { return getTransform().GetRotationEulerDegrees(); }
		const glm::vec3& getScales() const { return getTransform().GetScales(); }

		void SetPendingKill();
		bool IsPendingKill() const;

		void SetLifeTickCount(int32 t) { lifeTickCount_ = t; }
		// 여기 - lifetick time 을 만들어야한다

		// nsRE::WorldObject 와의 연동을 위하여 존재함
		void SetRendering(bool isr, bool propagate);
		bool IsRendering() const { return isRendering_; }

	public:
		// CActor interface
		virtual class CBoundComponent* GetMainBound();

	public:
		// CInt_Reflection
		virtual const CString& getClassRTTIName() const override { return This::GetClassRTTIName(); }
		virtual SGReflection& getReflection() override;
		virtual void OnAfterDeserialized() override;

		// CInt_Component
		virtual ComponentVector& getPersistentComponents() override;
		virtual ComponentVector& getTransientComponents() override;
		virtual ComponentVectorWeak& getAllComponents() override;

	protected:
		bool isPendingKill_ = false;
		bool isRendering_ = true;

		int32 lifeTickCount_ = 0;

		nsRE::Transform actorTransform_;

		CString actorStaticTag_;
		CString actorTag_;

	protected:
		CUniqPtr<TReflectionStruct> reflActor_;

		// CInt_Component
		ComponentVector persistentComponents_;
		ComponentVector transientComponents_;
		ComponentVectorWeak allComponents_;

		mutable class CBoundComponent* mainBoundCompo_;
		TActorKey actorKey_ = InvalidActorKey;
		CTimer timer_;

		friend class CSystemBase;	// for DynamicActorKey
	};
};
