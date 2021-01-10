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

		CWString GetGameAssetPath(const CWString& assetFilePath);
		CWString GetEngineAssetPath(const CWString& assetFilePath);

#if IS_EDITOR
		CWString GetGameProjectRoot();
#endif
	}

	namespace Configs
	{
		constexpr float BoundEpsilon = 0.001f;			// 점이나 선, 면 등을 사용할 때 어떤 두께가 필요한 경우가 있어서 정의된 상수, OpenGL 단위 기준 1 을 1미터로 놓고 1000으로 나눴다 - 즉 1미리미터임
		constexpr float BoundCheckEpsilon = 0.0001f;
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

			class nsGE::CEngineBase* GetEngine() const;

			virtual void Tick(float);

		protected:
			virtual void Initialize();

		protected:
			class nsGE::CEngineBase* engine_;
			SGEGameSettings* gameSettings_;

		public:
			static CGameBase* Instance;
		};
	}
};
