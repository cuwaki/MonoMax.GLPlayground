#pragma once

#include "GECommonIncludes.h"
#include "CUserInput.h"

namespace SMGE
{
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

		virtual void OnStartSystem();
		virtual void OnEndSystem();

		virtual bool ProcessUserInput();
		virtual void AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI);

		virtual void OnChangedSystemState(const CString& stateName);

	protected:
		class CEngineBase* engine_;

		std::vector<DELEGATE_ProcessUserInput> delegateUserInputs_;
	};
};
