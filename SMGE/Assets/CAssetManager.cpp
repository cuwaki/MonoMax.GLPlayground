#include "CAssetManager.h"
#include "../CGameBase.h"
#include "../GECommonIncludes.h"

namespace SMGE
{
	CHashMap<CWString, CSharPtr<CAssetBase>> CAssetManager::cachedAssets_;

	CWString CAssetManager::GetTemplateAssetPath(CWString className)
	{
		CuwakiDevUtils::ReplaceInline(className, wtext("SMGE::"), wtext(""));

		auto templateAssetPath = wtext("/templates/") + className + wtext(".asset");

		auto ret = Globals::GetGameAssetPath(templateAssetPath);
		return Path::GetNormalizedPath(ret);
	}
};