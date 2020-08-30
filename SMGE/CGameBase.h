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

			CWString PathProjectRoot();
			CWString PathAssetRoot();

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
