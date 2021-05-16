#include "CEngineBase.h"
#include "CGameBase.h"
#include "../MonoMax.EngineCore/RenderingEngine.h"

#if IS_EDITOR
#include "CSystemEditor.h"
#else
#include "CSystemGame.h"
#endif

namespace SMGE
{
	CEngineBase::CEngineBase(CGameBase* gameBase) : gameBase_(gameBase)
	{
#if IS_EDITOR
		system_ = std::make_unique<CSystemEditor>(this);
#else
		system_ = std::make_unique<CSystemGame>(this);
#endif
	}

	CEngineBase::~CEngineBase()
	{
		system_->OnDestroyingGameEngine();
	}

	HWND CEngineBase::GetFocusingWindow() const
	{
		assert(renderingEngine_ != nullptr);

#if IS_EDITOR
		const auto hwnd = ::GetActiveWindow();
#else
		const auto hwnd = renderingEngine_->GetHwndGLFWWindow();
#endif		
		if (hwnd == nullptr || ::GetFocus() != hwnd)
			return nullptr;

		return hwnd;
	}

	CUserInput& CEngineBase::GetUserInput()
	{
		return userInput_;
	}

	void CEngineBase::Tick(float timeDelta)
	{
		system_->Tick(timeDelta);
	}

	CSystemBase* CEngineBase::GetSystem() const
	{
		return system_.get();
	}

	void CEngineBase::SetRenderingEngine(nsRE::CRenderingEngine* re)
	{
		renderingEngine_ = re;

		if (renderingEngine_)
		{
			system_->OnLinkWithRenderingEngine();
		}
		else
		{
			assert(false && "never null");	// 현재로서는 이렇다 20210214
		}
	}

	nsRE::CRenderingEngine* CEngineBase::GetRenderingEngine() const
	{
		return renderingEngine_;
	}
}
