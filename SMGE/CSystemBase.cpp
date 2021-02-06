#include "CSystemBase.h"
#include "CEngineBase.h"

namespace SMGE
{
	//// deprecated 201227 - 카메라 액터 구현하면서 안쓰게 됨
	//auto EditorProcessCameraMove = [this](const CUserInput& userInput)
	//{
	//	auto& renderCam = GetRenderingEngine()->GetRenderingCamera();

	//	renderCam.MoveCamera(userInput.IsPressed(VK_LEFT), userInput.IsPressed(VK_RIGHT), userInput.IsPressed(VK_UP), userInput.IsPressed(VK_DOWN));

	//	static glm::vec2 RPressedPos;
	//	bool isJustRPress = userInput.IsJustPressed(VK_RBUTTON);
	//	if (isJustRPress)
	//		RPressedPos = userInput.GetMousePosition();

	//	bool isRPress = userInput.IsPressed(VK_RBUTTON);
	//	if (isJustRPress == false && isRPress == true)
	//	{
	//		auto moved = RPressedPos - userInput.GetMousePosition();
	//		renderCam.RotateCamera(moved);

	//		RPressedPos = userInput.GetMousePosition();
	//	}

	//	return false;
	//};
	//AddProcessUserInputs(EditorProcessCameraMove);

	CSystemBase::CSystemBase(class CEngineBase* engine) : engine_(engine)
	{
	}

	void CSystemBase::OnStartSystem()
	{

	}

	void CSystemBase::OnEndSystem()
	{

	}

	bool CSystemBase::ProcessUserInput()
	{
		auto& userInput = engine_->GetUserInput();

		userInput.QueryStateKeyOrButton();

		if (auto activeWindow = engine_->HasWindowFocus())
		{	// 포커스가 있을 때만
			userInput.QueryStateMousePos(activeWindow);

			for (auto& pui : delegateUserInputs_)
			{
				if (pui(userInput) == true)
					break;
			}
		}

		return true;
	}

	void CSystemBase::AddProcessUserInputs(const DELEGATE_ProcessUserInput& delegPUI)
	{
		delegateUserInputs_.push_back(delegPUI);
	}

	void CSystemBase::OnChangedSystemState(const CString& stateName)
	{

	}
};
