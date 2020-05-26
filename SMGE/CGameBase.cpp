#include "CGameBase.h"
#include "CEngineBase.h"

namespace SMGE
{
	namespace nsGE
	{
		CGameBase* CGameBase::Instance;

		CGameBase::CGameBase()
		{
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

#if IS_EDITOR
		void CGameBase::EditorTick(float timeDelta)
		{
			Tick(timeDelta);
		}

		void CGameBase::EditorRender(float timeDelta)
		{
			Render(timeDelta);
		}
#else
		int32 CGameBase::GameMain()
		{
			return 0;
		}
#endif

		void CGameBase::Tick(float timeDelta)
		{
			engine_->Tick(timeDelta);
		}

		void CGameBase::Render(float timeDelta)
		{
			engine_->Render(timeDelta);
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
