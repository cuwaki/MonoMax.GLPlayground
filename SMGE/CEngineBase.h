#pragma once

#include "GECommonIncludes.h"
#include "CGameBase.h"

// CUserInput
#include <vector>
#include <map>
#include <functional>

namespace SMGE
{
	namespace nsRE
	{
		class CRenderingEngine;
	}

	namespace nsGE
	{
		struct SGEEngineSettings
		{
			glm::vec2 gameScreenSize_{};
		};

		class CUserInput
		{
			using TInputKey = unsigned short;
			using TInputState = unsigned short;
			using TInputMap = std::map<TInputKey, TInputState>;

			// win32 - async inputs - https://m.blog.naver.com/power2845/50143021565

			static constexpr TInputState VK_RELEASED = 0x0000;		// 이전에 누른 적이 없고 호출 시점에도 눌려있지 않은 상태
			static constexpr TInputState VK_JUST_RELEASED = 0x0001;	// 이전에 누른 적이 있고 호출 시점에는 눌려있지 않은 상태
			static constexpr TInputState VK_PRESSED = 0x8001;		// 이전에 누른 적이 있고 호출 시점에도 눌려있는 상태
			static constexpr TInputState VK_JUST_PRESSED = 0x8000;	// 이전에 누른 적이 없고 호출 시점에는 눌려있는 상태

		public:
			// 리맵핑 가능하도록 static 이다
#if IS_EDITOR
			static TInputKey LBUTTON;
			static TInputKey RBUTTON;
			static TInputKey MBUTTON;
#else
#endif

		public:
			CUserInput();

			virtual void QueryState();
			bool HasInputFocus();

			bool IsPressed(TInputKey k) const
			{
				return CheckInputState(k, VK_PRESSED);
			}
			bool IsJustPressed(TInputKey k) const
			{
				return CheckInputState(k, VK_JUST_PRESSED);
			}
			bool IsReleased(TInputKey k) const
			{
				return CheckInputState(k, VK_RELEASED);
			}
			bool IsJustReleased(TInputKey k) const
			{
				return CheckInputState(k, VK_JUST_RELEASED);
			}
			const glm::vec2& GetMousePosition() const
			{
				return mousePos_;
			}			

		protected:
			bool CheckInputState(TInputKey k, TInputState state) const;

		protected:
			TInputMap checkMouseInputs_, checkMouseInputsOLD_;
			TInputMap checkKeyboardInputs_, checkKeyboardInputsOLD_;

			glm::vec2 mousePos_;
		};

		using DELEGATE_ProcessUserInput = std::function<bool(const CUserInput& userInput)>;	// return bool is break!

		class CEngineBase
		{
		public:
			CEngineBase(CGameBase* gameBase);
			virtual ~CEngineBase();

			void SetRenderingEngine(class nsRE::CRenderingEngine* re);
			class nsRE::CRenderingEngine* GetRenderingEngine() const;

			virtual void Tick(float);
			virtual void Render(float);

			void AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI);

		protected:
			CGameBase* gameBase_ = nullptr;
			class nsRE::CRenderingEngine* renderingEngine_ = nullptr;

			SGEEngineSettings* settings_;

			CUserInput userInput_;
			std::vector<DELEGATE_ProcessUserInput> delegateUserInputs_;
		};
	}
};
