#include "CGEGameBase.h"
#include "CGEEngineBase.h"

namespace MonoMaxGraphics
{
	CGEGameBase* CGEGameBase::Instance;

	CGEGameBase::CGEGameBase()
	{
		CGEGameBase::Instance = this;

		Initialize();
	}

	CGEGameBase::~CGEGameBase()
	{
		if(engine_)
			delete engine_;
		if (gameSettings_)
			delete gameSettings_;

		CGEGameBase::Instance = nullptr;
	}

	void CGEGameBase::Initialize()
	{
	}

#if IS_EDITOR
	void CGEGameBase::EditorTick(float timeDelta)
	{
		Tick(timeDelta);
	}

	void CGEGameBase::EditorRender(float timeDelta)
	{
		Render(timeDelta);
	}
#else
	int32 CGEGameBase::GameMain()
	{
		return 0;
	}
#endif

	void CGEGameBase::Tick(float timeDelta)
	{
		engine_->Tick(timeDelta);
	}

	void CGEGameBase::Render(float timeDelta)
	{
		engine_->Render(timeDelta);
	}

	CWString CGEGameBase::PathProjectRoot()
	{
		return gameSettings_->gameProjectRootPath_ + wtext("/");
	}

	CWString CGEGameBase::PathAssetRoot()
	{
		return PathProjectRoot() + wtext("/Assets/");
	}
};
