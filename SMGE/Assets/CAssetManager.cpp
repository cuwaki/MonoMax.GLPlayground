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
			// 테스트 코드
			pathes[wtext("SMGE::CActor")] = Path::GetNormalizedPath(pathAssetRoot + wtext("testActorTemplate.asset"));
			pathes[wtext("SMGE::CMap")] = Path::GetNormalizedPath(pathAssetRoot + wtext("/map/testMapTemplate.asset"));
			pathes[wtext("SMGE::CModelData")] = Path::GetNormalizedPath(pathAssetRoot + wtext("/mesh/testTriangle.asset"));
		}

		return pathes[className];
	}
};
