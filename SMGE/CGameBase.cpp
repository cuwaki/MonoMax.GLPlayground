#include "CGameBase.h"
#include "CEngineBase.h"

namespace SMGE
{
	namespace Globals
	{
		nsGE::CGameBase* GCurrentGame;

		CWString GetFullAssetPath(const CWString& assetFilePath)
		{
			return GetGameAssetPath() + assetFilePath;
		}
	}

	namespace nsGE
	{
		CGameBase* CGameBase::Instance;

		CGameBase::CGameBase(CObject* outer) : CObject(outer)
		{
			className_ = wtext("SMGE::nsGE::CGameBase");

			CGameBase::Instance = this;

			Initialize();
		}

		CGameBase::~CGameBase()
		{
			if (engine_)
				delete engine_;
			if (gameSettings_)
				delete gameSettings_;

			CGameBase::Instance = nullptr;
		}

		void CGameBase::Initialize()
		{
		}

		CEngineBase* CGameBase::GetEngine() const
		{
			return engine_;
		}

		void CGameBase::Tick(float timeDelta)
		{
		}

		void CGameBase::Render(float timeDelta)
		{
		}

		CWString CGameBase::PathProjectRoot()
		{
			return gameSettings_->gameProjectRootPath_ + wtext("/");
		}

		CWString CGameBase::PathAssetRoot()
		{
			return PathProjectRoot() + wtext("/Assets/");
		}
	}
};
