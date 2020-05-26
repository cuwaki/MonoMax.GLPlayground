#include "CAssetManager.h"
#include "../CGameBase.h"
#include "../GECommonIncludes.h"

namespace SMGE
{
	CHashMap<CWString, CSharPtr<CAssetBase>> CAssetManager::cachedAssets_;

	CWString FindAssetFilePathByClassName(CWString className)
	{
		static CHashMap<CWString, CWString> pathes;

		const auto pathAssetRoot = nsGE::CGameBase::Instance->PathAssetRoot();
		if (pathes.size() == 0)
		{
			// �׽�Ʈ �ڵ�
			pathes[wtext("SMGE_Game::CActor")] = SMGEGlobal::GetNormalizedPath(pathAssetRoot + wtext("testActorTemplate.asset"));
			pathes[wtext("SMGE_Game::CMap")] = SMGEGlobal::GetNormalizedPath(pathAssetRoot + wtext("/map/testMapTemplate.asset"));
			pathes[wtext("SMGE_Game::CModelData")] = SMGEGlobal::GetNormalizedPath(pathAssetRoot + wtext("/mesh/testTriangle.asset"));
		}

		return pathes[className];
	}
};
