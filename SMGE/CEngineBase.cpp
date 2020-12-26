#include "CEngineBase.h"
#include "CGameBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"

// CUserInput
#include <Windows.h>

namespace SMGE
{
	namespace nsGE
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
			checkKeyboardInputs_.insert(std::make_pair(VK_LEFT, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_RIGHT, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_UP, VK_RELEASED));
			checkKeyboardInputs_.insert(std::make_pair(VK_DOWN, VK_RELEASED));
#else
#endif
		}

		bool CUserInput::HasInputFocus()
		{
			HWND hwnd = ::GetActiveWindow();
			if (hwnd == nullptr || ::GetFocus() != hwnd)
				return false;

			return true;
		}

		void CUserInput::QueryState()
		{
#if IS_EDITOR
			HWND hwnd = ::GetActiveWindow();
			if (hwnd != nullptr && ::GetFocus() == hwnd)
			{
				POINT pos;
				::GetCursorPos(&pos);
				::ScreenToClient(hwnd, &pos);

				mousePos_ = { pos.x, pos.y };
			}

			// 포커스 없어도 해야한다
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
#endif
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
	}

	namespace nsGE
	{
		CEngineBase::CEngineBase(CGameBase* gameBase) : gameBase_(gameBase)
		{
			auto EditorProcessCameraMove = [this](const nsGE::CUserInput& userInput)
			{
				auto& renderCam = GetRenderingEngine()->GetRenderingCamera();

				renderCam.MoveCamera(userInput.IsPressed(VK_LEFT), userInput.IsPressed(VK_RIGHT),userInput.IsPressed(VK_UP), userInput.IsPressed(VK_DOWN));

				static glm::vec2 RPressedPos;
				bool isJustRPress = userInput.IsJustPressed(VK_RBUTTON);
				if (isJustRPress)
					RPressedPos = userInput.GetMousePosition();

				bool isRPress = userInput.IsPressed(VK_RBUTTON);
				if (isJustRPress == false && isRPress == true)
				{
					auto moved = RPressedPos - userInput.GetMousePosition();
					renderCam.RotateCamera(moved);

					RPressedPos = userInput.GetMousePosition();
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
			userInput_.QueryState();
			if (userInput_.HasInputFocus())
			{
				for (auto& pui : delegateUserInputs_)
				{
					if (pui(userInput_) == true)
						break;
				}
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
	{
		return CWString(astr.begin(), astr.end());
	}

	CString ToASCII(const CWString& wstr)
	{
		return CString(wstr.begin(), wstr.end());
	}

	namespace CuwakiDevUtils
	{
		void Quat2Direction(glm::quat& q, float& pitch, float& yaw, float& roll)
		{
			float test = q.x * q.y + q.z * q.w;
			if (test > 0.499) { // singularity at north pole
				yaw = 2 * atan2f(q.x, q.w);
				roll = 3.141592f / 2;
				pitch = 0;
				return;
			}
			if (test < -0.499) { // singularity at south pole
				yaw = -2 * atan2f(q.x, q.w);
				roll = -3.141592f / 2;
				pitch = 0;
				return;
			}
			float sqx = q.x * q.x;
			float sqy = q.y * q.y;
			float sqz = q.z * q.z;
			yaw = atan2f(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz);
			roll = asinf(2 * test);
			pitch = atan2f(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz);
		}
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
