#include "CGameBase.h"
#include "CEngineBase.h"
#include "Objects/CMap.h"

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
		CGameBase::Instance = nullptr;
	}

	void CGameBase::Initialize()
	{
	}

	CEngineBase* CGameBase::GetEngine() const
	{
		return engine_.get();
	}

	void CGameBase::Tick(float timeDelta)
	{
		engine_->Tick(timeDelta);
	}
};
