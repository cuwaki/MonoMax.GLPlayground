#pragma once

#include "GECommonIncludes.h"

namespace MonoMaxGraphics
{
	struct SGEGameSettings
	{
		CWString gameProjectName_;
		CWString gameProjectRootPath_;
	};

	class CGEGameBase
	{
	public:
		CGEGameBase();
		virtual ~CGEGameBase();

		virtual void Initialize();
		virtual void Tick(float timeDelta);

		CWString PathProjectRoot();
		CWString PathAssetRoot();

	protected:
		class CGEEngineBase* engine_;
		SGEGameSettings* gameSettings_;

	public:
		static CGEGameBase* Instance;
	};
};
