#pragma once

#include "GECommonIncludes.h"

namespace SMGE
{
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

		virtual void QueryStateMousePos(HWND activeWindow);
		virtual void QueryStateKeyOrButton();

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
};
