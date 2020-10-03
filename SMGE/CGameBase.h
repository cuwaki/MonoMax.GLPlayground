#pragma once

#include "GECommonIncludes.h"
#include "Objects/CObject.h"
namespace SMGE
{
	namespace nsGE
	{
		class CGameBase;
	}

	namespace Globals
	{
		extern nsGE::CGameBase* GCurrentGame;

		CWString GetGameProjectName();

		CWString GetGameAssetRoot();
		\
			CWString GetGameAssetPath(const CWString& assetFilePath);
		CWString GetEngineAssetPath(const CWString& assetFilePath);

#if IS_EDITOR
		CWString GetGameProjectRoot();
#endif
	}

	namespace Configs
	{
		constexpr static float BoundEpsilon = 0.0001f;	// 점이나 선, 면 등을 사용할 때 어떤 두께가 필요한 경우가 있어서 정의된 상수
	}

	namespace nsGE
	{
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
			virtual void Render(float);

		protected:
			virtual void Initialize();

		protected:
			class CEngineBase* engine_;
			SGEGameSettings* gameSettings_;

		public:
			static CGameBase* Instance;
		};
	}
};
