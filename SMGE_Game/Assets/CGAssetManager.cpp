#include "CGAssetManager.h"
#include "../SMGE/CGEGameBase.h"
#include "../SMGE/GECommonIncludes.h"

namespace MonoMaxGraphics
{
	CMap<CWString, CSharPtr<CGAssetBase>> CGAssetManager::cachedAssets_;

	CWString FindAssetFilePathByClassName(CWString className)
	{
		static CMap<CWString, CWString> pathes;

		const auto pathAssetRoot = CGEGameBase::Instance->PathAssetRoot();
		if (pathes.size() == 0)
		{
			pathes[wtext("SMGE_Game::CGActor")] = SMGEGlobal::GetNormalizedPath(pathAssetRoot + wtext("testActorTemplate.asset"));
			pathes[wtext("SMGE_Game::CGMap")] = SMGEGlobal::GetNormalizedPath(pathAssetRoot + wtext("/map/testMapTemplate.asset"));
		}

		return pathes[className];
	}
};
