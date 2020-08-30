#pragma once

#include "CObject.h"
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
		SGRefl_Actor(const CSharPtr<CActor>& actorPtr) : SGRefl_Actor(*actorPtr) {}

		// { persistentComponentsREFL_ RTTI 필요 이슈
		void linkINST2REFL();

		virtual void buildVariablesMap() override;
		
		virtual void OnBeforeSerialize() const override;
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
		SGRefl_Transform sg_actorTransform_;
		CString& actorStaticTag_;

		// { persistentComponentsREFL_ RTTI 필요 이슈
		//CVector<SGRefl_Component> persistentComponentsREFL_;
		
		// { persistentComponentsREFL_ RTTI 필요 이슈 - 아래처럼 mutable 로 처리해놨다 / 이거 어떡하지?
		mutable int32_t persistentComponentNumber_ = 0;

		// for Runtime
		CActor& outerActor_;
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

		void Ctor();
		void Dtor();

		virtual void Tick(float);	// 차후 개발 - CGInterf_Tickable 로 빼자
		virtual void Render(float);	// 차후 개발 - CGInterf_Renderable 로 빼자???

		nsRE::Transform& getTransform();
		const nsRE::Transform& getTransform() const;

		void setActorStaticTag(const CString& st) { actorStaticTag_ = st; }
		CString getActorStaticTag() { return actorStaticTag_; }

		virtual void OnAfterSpawned(class CMap* map, bool isDynamic);
		virtual void OnAfterArranged(class CMap* map);
		virtual void BeginPlay();
		virtual void EndPlay();

		CTimer& getActorTimer() { return timer_; }

		const glm::vec3& getLocation() const {	return getTransform().GetTranslation(); }
		const glm::vec3& getRotation() const { return getTransform().GetRotation(); }
		const glm::vec3& getScale() const { return getTransform().GetScale(); }

		const class CBoundComponent* GetMainBound() const;

	public:
		// CInt_Reflection
		virtual const CWString& getClassName() override { return className_; }
		virtual SGReflection& getReflection() override;
		virtual void OnAfterDeserialized() override;

		// CInt_Component
		virtual ComponentVector& getPersistentComponents() override;
		virtual ComponentVector& getTransientComponents() override;
		virtual ComponentVectorWeak& getAllComponents() override;

	protected:
		nsRE::Transform actorTransform_;

		CString actorStaticTag_;
		CString actorTag_;

		// 여기 생각 - mainDrawCompo 는 persistcomp 라서 애셋으로부터 동적으로 생성될 수 있어서 멤버 포인터를 적용할 수가 없었다, 하지만 무브먼트콤포는??? 생각해볼 것, 지금은 트랜젼트임
		class CMovementComponent* movementCompo_;
		class CBoundComponent* mainBoundCompo_;

	protected:
		CUniqPtr<TReflectionStruct> reflActor_;

		// CInt_Component
		ComponentVector persistentComponents_;
		ComponentVector transientComponents_;
		ComponentVectorWeak allComponents_;

		TActorKey actorKey_ = InvalidActorKey;

		CTimer timer_;

		// ActorKeyGenerator
		friend class CMap;
	};



	class CCollideActor : public CActor
	{
		using Super = CActor;

	public:
		CCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide);

		virtual void BeginPlay() override;

		virtual std::vector<CActor*> QueryCollideCheckTargets() = 0;
		virtual void ProcessCollide(ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, std::vector<CActor*>& targets) = 0;

	protected:
		ECheckCollideRule rule_;
		bool isDetailCheck_;
		const DELEGATE_OnCollide& fOnCollide_;
	};

	class CRayCollideActor : public CCollideActor
	{
		using Super = CCollideActor;

	public:
		CRayCollideActor(CObject* outer, ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide);

		void Ctor();

		virtual std::vector<CActor*> QueryCollideCheckTargets() override;
		virtual void ProcessCollide(ECheckCollideRule rule, bool isDetailCheck, const DELEGATE_OnCollide& fOnCollide, std::vector<CActor*>& targets) override;
	};
};
