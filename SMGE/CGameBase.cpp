#include "CGameBase.h"
#include "CEngineBase.h"

namespace SMGE
{
	namespace Globals
	{
		CGameBase* GCurrentGame;

		CWString GetGameAssetPath(const CWString& assetFilePath)
		{
			return GetGameAssetRoot() + assetFilePath;
		}
	}

	CGameBase* CGameBase::Instance;

	CGameBase::CGameBase(CObject* outer) : CObject(outer)
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

	CEngineBase* CGameBase::GetEngine() const
	{
		return engine_;
	}

	void CGameBase::Tick(float timeDelta)
	{
		engine_->Tick(timeDelta);
	}
};
