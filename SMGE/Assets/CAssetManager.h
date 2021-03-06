#pragma once

#include "../GECommonIncludes.h"
#include "CAsset.h"

namespace SMGE
{
	class CAssetManager
	{
	public:
		static CWString GetTemplateAssetPath(CWString className);

		template<typename C>
		static std::shared_ptr<CAsset<C>> FindAsset(CWString filePath)
		{
			ToLowerInline(filePath);

			auto res = cachedAssets_.find(filePath);
			if (res != cachedAssets_.end())
				return std::static_pointer_cast<CAsset<C>>(res->second);

			return nullptr;
		}

		template<typename C>
		static std::shared_ptr<CAsset<C>> LoadAsset(CWString filePath)
		{
			ToLowerInline(filePath);

			auto res = FindAsset<C>(filePath);
			if (res)
				return res;

			cachedAssets_[filePath] = std::move(std::make_shared<CAsset<C>>(filePath));
			return std::static_pointer_cast<CAsset<C>>(cachedAssets_[filePath]);
		}

		template<typename C>
		static bool SaveAsset(CWString filePath, CAsset<C>& target)
		{
			SGStringStreamOut strOut(target.getContentClass()->getReflection());

			return SaveToTextFile(filePath, strOut.out_);
		}

	protected:
		static CHashMap<CWString, std::shared_ptr<CAssetBase> > cachedAssets_;
	};
}
