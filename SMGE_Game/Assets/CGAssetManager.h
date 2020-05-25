#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "CGAsset.h"

namespace SMGE
{
	CWString FindAssetFilePathByClassName(CWString className);

	class CGAssetManager
	{
	public:
		static CWString FindAssetFilePathByClassName(CWString className)
		{
			return SMGE::FindAssetFilePathByClassName(className);
		}

		template<typename C>
		static CSharPtr<CGAsset<C>> FindAsset(CWString filePath)
		{
			ToLowerInline(filePath);

			auto res = cachedAssets_.find(filePath);
			if (res != cachedAssets_.end())
				return std::static_pointer_cast<CGAsset<C>>(res->second);

			return nullptr;
		}

		template<typename C>
		static CSharPtr<CGAsset<C>> LoadAsset(CWString filePath)
		{
			ToLowerInline(filePath);

			auto res = FindAsset<C>(filePath);
			if (res)
				return res;

			cachedAssets_[filePath] = std::move(MakeSharPtr<CGAsset<C>>(filePath));
			return SPtrCast<CGAsset<C>>(cachedAssets_[filePath]);
		}

		template<typename C>
		static bool SaveAsset(CWString filePath, CGAsset<C>& target)
		{
			SGStringStreamOut strOut(target.getContentClass()->getReflection());

			return CuwakiDevUtils::SaveToTextFile(filePath, strOut.out_);
		}

		static CMap<CWString, CSharPtr<CGAssetBase>> cachedAssets_;
	};
}
