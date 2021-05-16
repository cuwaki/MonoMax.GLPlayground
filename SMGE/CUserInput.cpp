#include "CUserInput.h"

namespace SMGE
{
	CUserInput::TInputKey CUserInput::LButton = VK_LBUTTON;
	CUserInput::TInputKey CUserInput::RButton = VK_RBUTTON;
	CUserInput::TInputKey CUserInput::MButton = VK_MBUTTON;
	CUserInput::TInputKey CUserInput::AddMultipleKey = VK_CONTROL;

	CUserInput::CUserInput()
	{
#if IS_EDITOR
		checkMouseInputs_.insert(std::make_pair(VK_LBUTTON, VK_RELEASED));
		checkMouseInputs_.insert(std::make_pair(VK_RBUTTON, VK_RELEASED));
		checkMouseInputs_.insert(std::make_pair(VK_MBUTTON, VK_RELEASED));

		checkKeyboardInputs_.insert(std::make_pair(VK_SPACE, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_RETURN, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_ESCAPE, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_CONTROL, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_MENU, VK_RELEASED));
		checkKeyboardInputs_.insert(std::make_pair(VK_SHIFT, VK_RELEASED));
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
			// 드래그 처리
			for (auto& it : checkMouseInputs_)
			{
				const auto k = it.first;
				if (IsDragging(k))
				{
					prevTickPos_[k] = mousePos_;	// old pos 저장한 셈이 된다
				}
			}

			POINT pos;
			::GetCursorPos(&pos);
			::ScreenToClient(activeWindow, &pos);

			mousePos_ = { pos.x, pos.y };

			// 드래그 처리
			for (auto& it : checkMouseInputs_)
			{
				const auto k = it.first;

				if (CheckInputState(k, VK_JUST_PRESSED))
				{
					prevTickPos_[k] = mousePos_;	// 드래그를 시작한 위치를 저장하는 셈이 된다
				}
				else if (CheckInputState(k, VK_JUST_RELEASED))
				{
					prevTickPos_[k] = glm::vec2(0);
					isDragging_[k] = false;
				}
				else if (CheckInputState(k, VK_PRESSED))
				{
					if (isDragging_[k] == false)
					{	// 여기선 한번 켜졌으면 끄지 않는다, 드래그 하다가 같은 자리로 돌아올 수도 있으니까
						const auto moved = prevTickPos_[k] - mousePos_;	// 최초 눌렸던 위치와 현재 위치의 차이
						if (glm::length2(moved) >= 1.f)	// 1 보다 크면 이니까 length2 로 해도 된다???
						{
							isDragging_[k] = true;
							wasDragging_[k] = false;
						}
					}
					else
					{	// 전 틱에 드래깅 시작했으니까
						wasDragging_[k] = true;
					}
				}
				else
				{
					isDragging_[k] = false;
					wasDragging_[k] = false;
				}
			}
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
		const TInputStateMap* checkInputs[4] = { &checkMouseInputs_, &checkMouseInputsOLD_,
												 &checkKeyboardInputs_, &checkKeyboardInputsOLD_ };

		for (int i = 0; i < 4; i += 2)
		{
			const auto& neww = checkInputs[i + 0];

			const auto found = neww->find(k);
			if (found != neww->end())
			{
				const auto& old = checkInputs[i + 1];

				const auto oldState = old->find(k)->second, newState = neww->find(k)->second;
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

	void CUserInput::Tick(HWND focusingWindow)
	{
		QueryStateKeyOrButton();

		if (focusingWindow)
		{	// 포커스가 있을 때만
			QueryStateMousePos(focusingWindow);
		}
	}
};
