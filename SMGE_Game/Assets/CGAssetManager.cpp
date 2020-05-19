#include "CGAssetManager.h"

namespace MonoMaxGraphics
{
	CMap<CWString, CSharPtr<CGAssetBase>> CGAssetManager::cachedAssets_;

	CWString FindAssetFilePathByClassName(CWString className)
	{
		static CMap<CWString, CWString> pathes;

		if (pathes.size() == 0)
		{
			pathes[wtext("SMGE_Game::CGActor")] = wtext("testActorTemplate.asset");
			pathes[wtext("SMGE_Game::CGMap")] = wtext("/map/testMapTemplate.asset");
		}

		return pathes[className];
	}
};
