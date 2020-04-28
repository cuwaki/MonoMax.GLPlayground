#include "CGAssetManager.h"

namespace MonoMaxGraphics
{
	CMap<CWString, CSharPtr<CGAsset>> CGAssetManager::allAssetMap_;

	CSharPtr<CGAsset> CGAssetManager::FindAsset(CWString filePath)
	{
		ToLower(filePath);

		auto res = allAssetMap_.find(filePath);
		if (res != allAssetMap_.end())
			return res->second;

		return nullptr;
	}

	template<typename T>
	CSharPtr<T> CGAssetManager::LoadAsset(CWString filePath)
	{
		auto res = FindAsset(filePath);
		if (res)
			return res;

		allAssetMap_[filePath] = std::move(MakeSharPtr<T>());
		return allAssetMap_[filePath];
	}
};
