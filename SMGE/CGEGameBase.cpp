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

	void CGEGameBase::Tick(float timeDelta)
	{
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
