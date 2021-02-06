#include "CUserInput.h"

namespace SMGE
{
	CUserInput::TInputKey CUserInput::LBUTTON = VK_LBUTTON;
	CUserInput::TInputKey CUserInput::RBUTTON = VK_RBUTTON;
	CUserInput::TInputKey CUserInput::MBUTTON = VK_MBUTTON;

	CUserInput::CUserInput()
	{
#if IS_EDITOR
		checkMouseInputs_.insert(std::make_pair(VK_LBUTTON, VK_RELEASED));
		checkMouseInputs_.insert(std::make_pair(VK_RBUTTON, VK_RELEASED));
		checkMouseInputs_.insert(std::make_pair(VK_MBUTTON, VK_RELEASED));

		checkKeyboardInputs_.insert(std::make_pair(VK_SPACE, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_RETURN, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_ESCAPE, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair('A', VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair('D', VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair('W', VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair('S', VK_RELEASED));
#else
#endif
	}

	void CUserInput::QueryStateMousePos(HWND activeWindow)
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(::GetFocus() == activeWindow);	// 포커스가 맞을 때만 하도록 보장해라
#endif

		if (activeWindow != nullptr)
		{
			POINT pos;
			::GetCursorPos(&pos);
			::ScreenToClient(activeWindow, &pos);

			mousePos_ = { pos.x, pos.y };
		}
	}

	void CUserInput::QueryStateKeyOrButton()
	{
		for (auto& it : checkMouseInputs_)
		{
			checkMouseInputsOLD_[it.first] = it.second;
			it.second = ::GetAsyncKeyState(it.first);
		}
		for (auto& it : checkKeyboardInputs_)
		{
			checkKeyboardInputsOLD_[it.first] = it.second;
			it.second = ::GetAsyncKeyState(it.first);
		}
	}

	bool CUserInput::CheckInputState(TInputKey k, TInputState state) const
	{
		const TInputMap* checkInputs[4] = { &checkMouseInputs_, &checkMouseInputsOLD_,
											&checkKeyboardInputs_, &checkKeyboardInputsOLD_ };

		for (int i = 0; i < 4; i += 2)
		{
			auto& neww = checkInputs[i + 0];

			auto found = neww->find(k);
			if (found != neww->end())
			{
				auto& old = checkInputs[i + 1];

				auto oldState = old->find(k)->second, newState = neww->find(k)->second;
				switch (state)
				{
				case VK_JUST_PRESSED:
					if ((oldState & 0x8000) == 0 && (newState & 0x8000) == 0x8000)
						return true;
					else
						return false;

				case VK_PRESSED:
					if ((oldState & 0x8000) == 0x8000 && (newState & 0x8000) == 0x8000)
						return true;
					else
						return false;

				case VK_JUST_RELEASED:
					if ((oldState & 0x8000) == 0x8000 && (newState & 0x8000) == 0)
						return true;
					else
						return false;

				case VK_RELEASED:
					if ((oldState & 0x8000) == 0 && (newState & 0x8000) == 0)
						return true;
					else
						return false;
				}
			}
		}

		return false;
	}
};
