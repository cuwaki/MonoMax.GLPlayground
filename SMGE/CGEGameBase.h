#pragma once

#include "GECommonIncludes.h"

#define IS_EDITOR 1

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

#if IS_EDITOR
		void EditorTick(float timeDelta);
		void EditorRender(float timeDelta);
#else
		int32 GameMain();
#endif

		CWString PathProjectRoot();
		CWString PathAssetRoot();

	protected:
		virtual void Initialize();
		virtual void Tick(float);
		virtual void Render(float);

	protected:
		class CGEEngineBase* engine_;
		SGEGameSettings* gameSettings_;

	public:
		static CGEGameBase* Instance;
	};
};
