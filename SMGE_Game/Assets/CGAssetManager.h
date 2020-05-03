#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "CGAsset.h"

namespace MonoMaxGraphics
{
	class CGAssetManager
	{
	public:
		template<typename C>
		static CSharPtr<CGAsset<C>> FindAsset(CWString filePath)
		{
			ToLower(filePath);

			auto res = allAssetMap_.find(filePath);
			if (res != allAssetMap_.end())
				return std::static_pointer_cast<CGAsset<C>>(res->second);

			return nullptr;
		}

		template<typename C>
		static CSharPtr<CGAsset<C>> LoadAsset(CWString filePath)
		{
			auto res = FindAsset<C>(filePath);
			if (res)
				return res;

			allAssetMap_[filePath] = std::move(MakeSharPtr<CGAsset<C>>());
			return std::static_pointer_cast<CGAsset<C>>(allAssetMap_[filePath]);
		}

		template<typename C>
		static CSharPtr<CGAsset<C>> SaveAsset(CWString filePath, const CGAsset<C>& target)
		{
			CWString strToFile = target.getConstReflection();

			// save to file

			return LoadAsset<C>(filePath);
		}

		static CMap<CWString, CSharPtr<CGAssetBase>> allAssetMap_;
	};
}
