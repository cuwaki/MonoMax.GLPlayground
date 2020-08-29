#include "CAssetManager.h"
#include "../CGameBase.h"
#include "../GECommonIncludes.h"

namespace SMGE
{
	CHashMap<CWString, CSharPtr<CAssetBase>> CAssetManager::cachedAssets_;

	CWString CAssetManager::GetTemplateAssetPath(CWString className)
	{
		CuwakiDevUtils::ReplaceInline(className, wtext("SMGE::"), wtext(""));

		auto ret = nsGE::CGameBase::Instance->PathAssetRoot();
		ret += wtext("/templates/");
		ret += className;
		ret += wtext(".asset");

		return Path::GetNormalizedPath(ret);

		//static CHashMap<CWString, CWString> pathes;
		//const auto pathAssetRoot = nsGE::CGameBase::Instance->PathAssetRoot();
		//if (pathes.size() == 0)
		//{
		//	pathes[wtext("SMGE::CActor")] = Path::GetNormalizedPath(pathAssetRoot + wtext("/templates/CActor.asset"));
		//	pathes[wtext("SMGE::CMap")] = Path::GetNormalizedPath(pathAssetRoot + wtext("/templates/CMap.asset"));
		//	pathes[wtext("SMGE::CResourceModel")] = Path::GetNormalizedPath(pathAssetRoot + wtext("/templates/CResourceModel.asset"));
		//}

		//return pathes[className];
	}
};