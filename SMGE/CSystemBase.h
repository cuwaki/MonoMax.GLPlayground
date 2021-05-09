#pragma once

#include "GECommonIncludes.h"
#include "CUserInput.h"
#include "Objects/CActor.h"
#include "RTTI.hpp"

namespace SMGE
{
	namespace nsRE
	{
		class CRenderingEngine;
	}

	/*
	* 시스템
	* 
	* 게임 엔진이 이 시스템을 활용하여 에디터와 게임을 각기 다르게 처리하게 된다.
	* 단, 게임 시스템은 게임의 로직을 구현하는 것이 아니다!!
	* 말 그대로 게임 시스템은 게임이 돌아가기 위해 엔진과 협동하는 부분만이 그 구현의 대상이 된다.
	* 게임의 로직은 CGameRuleBase 를 상속받은 객체에 구현하도록 하자.
	* 
	* 객체의 이름이 Operator 가 나을 수도 있어보인다, 시스템보다 덜 보편적이고, 조금 더 어울리는 듯?
	*/
	class CSystemBase
	{
	public:
		CSystemBase(class CEngineBase *engine);

		virtual void Tick(float timeDelta);

		virtual void OnLinkWithRenderingEngine();
		virtual void OnDestroyingGameEngine();

		virtual bool ProcessUserInput();
		virtual void AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI);

		virtual void OnChangedSystemState(const CString& stateName);

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// CActor 관리
		//CVector<std::unique_ptr<CActor>>& GetAllActors() { return allActors_; }	// 허용하면 안된다
		const CVector<std::unique_ptr<CActor>>& GetAllActors() const { return allActors_; }

		virtual void ProcessPendingKill(class CActor* actor);
		virtual void ProcessPendingKills();
		virtual CActor& StartSpawnActorINTERNAL(class CMap* targetMap, class CObject* newObj, bool isDynamic);

		// 애셋등에서 리플렉션으로 액터를 생성할 때 사용
		template<typename... Args>
		CActor& StartSpawnActorDEFAULT(class CMap* targetMap, const std::string& classRTTIName, bool isDynamic, Args_START Args&&... args)
		{
			auto newObj = RTTI_CObject::NewDefault(classRTTIName, Args_START std::forward<Args>(args)...);
			return static_cast<CActor&>(StartSpawnActorINTERNAL(targetMap, newObj, isDynamic));
		}

		// 코드에서 하드코딩으로 액터를 스폰할 때 사용
		template<typename ActorT, typename... Args>
		ActorT& StartSpawnActorVARIADIC(class CMap* targetMap, bool isDynamic, Args_START Args&&... args)
		{
			auto newObj = RTTI_CObject::NewVariadic<ActorT>(Args_START std::forward<Args>(args)...);
			return static_cast<ActorT&>(StartSpawnActorINTERNAL(targetMap, newObj, isDynamic));
		}

		CActor* FinishSpawnActor(class CMap* targetMap, CActor* arrangedActor);

		//std::unique_ptr<CActor>&& RemoveActor(TActorKey ak);

	protected:
		class CEngineBase* engine_;
		class nsRE::CRenderingEngine* renderingEngine_;

		CVector<std::unique_ptr<CActor>> allActors_;
		CHashMap<CActor*, class CMap*> actorsMap_;

		std::vector<DELEGATE_ProcessUserInput> delegateUserInputs_;

	protected:
		static TActorKey DynamicActorKey;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CRenderPassWithSystem : public nsRE::CRenderingPass
	{
	public:
		CRenderPassWithSystem(CSystemBase* system) : system_(system)
		{
		}

		CSystemBase* GetSystem() const { return system_; }

	protected:
		CSystemBase* system_ = nullptr;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CRenderPassWorld : public CRenderPassWithSystem
	{
	public:
		CRenderPassWorld(CSystemBase* system);
		virtual void RenderTo(const glm::mat4& V, const glm::mat4& VP, nsRE::CRenderTarget*& writeRT, nsRE::CRenderTarget*& readRT) override;
	};
};
