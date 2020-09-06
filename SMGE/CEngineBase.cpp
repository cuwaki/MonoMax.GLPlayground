#include "CEngineBase.h"
#include "CGameBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"

// CUserInput
#include <Windows.h>

namespace SMGE
{
	namespace nsGE
	{
		CUserInput::TInputKey CUserInput::M_LBUTTON = VK_LBUTTON;
		CUserInput::TInputKey CUserInput::M_RBUTTON = VK_RBUTTON;
		CUserInput::TInputKey CUserInput::M_MBUTTON = VK_MBUTTON;

		CUserInput::CUserInput()
		{
#if IS_EDITOR
			checkMouseInputs_.insert(std::make_pair(VK_LBUTTON, VK_RELEASED));
			checkMouseInputsOLD_.insert(std::make_pair(VK_LBUTTON, VK_RELEASED));
			//checkMouseInputs_.insert(std::make_pair(VK_RBUTTON, VK_RELEASED));
			//checkMouseInputs_.insert(std::make_pair(VK_MBUTTON, VK_RELEASED));

			checkKeyboardInputs_.insert(std::make_pair(VK_SPACE, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_RETURN, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_ESCAPE, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_LEFT, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_RIGHT, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_UP, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_DOWN, VK_RELEASED));
#else
#endif
		}

		void CUserInput::Tick()
		{
			HWND hwnd = ::GetActiveWindow();
			if (hwnd != nullptr)
			{
				POINT pos;
				::GetCursorPos(&pos);
				::ScreenToClient(hwnd, &pos);
				
				mousePos_ = { pos.x, pos.y };
			}

			for (auto& it : checkMouseInputs_)
			{
				checkMouseInputsOLD_.find(it.first)->second = it.second;
				it.second = ::GetAsyncKeyState(it.first);
			}
			for (auto& it : checkKeyboardInputs_)
			{
				it.second = ::GetAsyncKeyState(it.first);
			}
		}

		bool CUserInput::CheckInputState(TInputKey k, TInputState state) const
		{
			auto found = checkMouseInputs_.find(k);
			if (found != checkMouseInputs_.end())
			{
				auto old = checkMouseInputsOLD_.find(k)->second, neww = checkMouseInputs_.find(k)->second;

				switch (state)
				{
				case VK_JUST_PRESSED:
					if ((old & 0x8000) != 0x8000 && (neww & 0x8000) == 0x8000)
						return true;
					else
						return false;

				case VK_PRESSED:
					if ((old & 0x8000) == 0x8000 && (neww & 0x8000) == 0x8000)
						return true;
					else
						return false;

				case VK_JUST_RELEASED:
					break;
				}
			}

			found = checkKeyboardInputs_.find(k);
			if (found != checkKeyboardInputs_.end())
				return ((*found).second & state) == state;

			return false;
		}
	}

	namespace nsGE
	{
		CEngineBase::CEngineBase(CGameBase* gameBase) : gameBase_(gameBase)
		{
			auto EditorProcessCameraMove = [this](const nsGE::CUserInput& userInput)
			{
				GetRenderingEngine()->GetCamera()->MoveCamera(
					userInput.IsPressed(VK_LEFT), userInput.IsPressed(VK_RIGHT),
					userInput.IsPressed(VK_UP), userInput.IsPressed(VK_DOWN));

				static glm::vec2 lPressedPos;
				bool isJustLPress = userInput.IsJustPressed(VK_LBUTTON);
				if (isJustLPress)
					lPressedPos = userInput.GetMousePosition();

				bool isLPress = userInput.IsPressed(VK_LBUTTON);
				if (isJustLPress == false && isLPress == true)
				{
					auto moved = lPressedPos - userInput.GetMousePosition();
					GetRenderingEngine()->GetCamera()->RotateCamera(moved);

					lPressedPos = userInput.GetMousePosition();
				}

				return false;
			};
			AddProcessUserInputs(EditorProcessCameraMove);
		}

		CEngineBase::~CEngineBase()
		{
		}

		void CEngineBase::Tick(float timeDelta)
		{
			userInput_.Tick();
			for (auto& pui : delegateUserInputs_)
			{
				if (pui(userInput_) == true)
					break;
			}

			gameBase_->Tick(timeDelta);
		}

		void CEngineBase::Render(float timeDelta)
		{
			gameBase_->Render(timeDelta);
		}

		void CEngineBase::SetRenderingEngine(nsRE::CRenderingEngine* re)
		{
			renderingEngine_ = re;
		}

		nsRE::CRenderingEngine* CEngineBase::GetRenderingEngine()
		{
			return renderingEngine_;
		}

		void CEngineBase::AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI)
		{
			delegateUserInputs_.push_back(delegPUI);
		}
	}
}

// 차후 할 일
#include <locale>
#include <codecvt>
#include <string>
#include "GEContainers.h"

// deprecated c++17
//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//std::string narrow = converter.to_bytes(wide_utf16_source_string);
//std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

namespace SMGE
{
	CWString ToTCHAR(const CString& astr)
	{	// 추가 할 일 - 뭔가 제대로 안될 것 같다
		//CWString ret = converter.from_bytes(astr);
		CWString ret(astr.begin(), astr.end());
		//ret.assign(astr.begin(), astr.end());
		return ret;
	}

	CString ToASCII(const CWString& wstr)
	{	// 추가 할 일 - 뭔가 제대로 안될 것 같다
		//CString ret = converter.to_bytes(wstr);
		CString ret(wstr.begin(), wstr.end());
		//ret.assign;
		return ret;
	}

	namespace Path
	{
		CWString GetNormalizedPath(const CWString& path)
		{
			CWString ret = CuwakiDevUtils::Replace(path, L"\\", L"/");
			return CuwakiDevUtils::Replace(ret, L"//", L"/");	// 차후 - 없을 때까지 제대로 처리하자
		}

		bool IsValidPath(const CWString& path)
		{
			return path.length() > 0 && (path.find(L'/') != CWString::npos || path.find(L'\\') != CWString::npos);
		}

		CWString GetDirectoryFullPath(const CWString& dir)
		{
			wchar full[_MAX_PATH];
			if (_wfullpath(full, dir.c_str(), _MAX_PATH) != nullptr)
				return GetNormalizedPath(full);

			return wtext("error_path");
		}

		CWString GetDirectoryCurrent()
		{
			return GetDirectoryFullPath(wtext("./"));
		}
	}

	namespace GlobalUtils
	{
		bool IsContains(const CWString& str, const CWString& finding)
		{
			return str.find(finding) != CWString::npos;
		}
		bool IsStartsWith(const CWString& str, const CWString& finding)
		{
			return str.find(finding) == 0;
		}
	}
}
