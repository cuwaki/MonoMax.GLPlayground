#pragma once

#include "GECommonIncludes.h"
#include "Objects/CObject.h"

namespace SMGE
{
	class CGameBase;

	namespace Globals
	{
		extern CGameBase* GCurrentGame;

		CWString GetGameProjectName();

		CWString GetGameAssetRoot();

		CWString GetGameAssetPath(const CWString& assetFilePath);
		CWString GetEngineAssetPath(const CWString& assetFilePath);

#if IS_EDITOR
		CWString GetGameProjectRoot();
#endif
	}

	struct SGEGameSettings
	{
		CWString gameProjectName_;
		CWString gameProjectRootPath_;
	};

	class CGameBase : public CObject
	{
	public:
		CGameBase(CObject* outer);
		virtual ~CGameBase();

		class CEngineBase* GetEngine() const;

		virtual void Tick(float);

	protected:
		virtual void Initialize();

	protected:
		std::unique_ptr<class CEngineBase> engine_;
		std::unique_ptr<SGEGameSettings> gameSettings_;

		// 아키텍트적으로는 맵을 감싸는 게임서버 객체가 필요하겠지만 이는 차후에 처리하자
		std::unique_ptr<class CMap> currentMap_;

	public:
		static CGameBase* Instance;
	};
};
