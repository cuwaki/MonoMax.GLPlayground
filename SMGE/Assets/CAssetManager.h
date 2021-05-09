#pragma once

#include "../GECommonIncludes.h"
#include "../Objects/CObject.h"
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

		template<typename C, typename ...Args>
		static std::shared_ptr<CAsset<C>> LoadAssetDefault(CWString filePath, Args&&... contentClassCtorArgs)
		{
			ToLowerInline(filePath);

			auto res = FindAsset<C>(filePath);
			if (res)
				return res;

			auto newAsset = std::make_shared<CAsset<C>>(filePath, std::forward<Args>(contentClassCtorArgs)...);
			cachedAssets_[filePath] = std::move(newAsset);

			return std::static_pointer_cast<CAsset<C>>(cachedAssets_[filePath]);
		}

		template<typename C, typename NewContentF, typename DeleteContentF, typename ...Args>
		static std::shared_ptr<CAsset<C>> LoadAssetCustom(CWString filePath, NewContentF&& nf, DeleteContentF&& df, Args&&... contentClassCtorArgs)
		{
			ToLowerInline(filePath);

			auto res = FindAsset<C>(filePath);
			if (res)
				return res;

			auto newAsset = std::make_shared<CAsset<C>>(filePath, std::forward<NewContentF>(nf), std::forward<DeleteContentF>(df), std::forward<Args>(contentClassCtorArgs)...);
			cachedAssets_[filePath] = std::move(newAsset);

			return std::static_pointer_cast<CAsset<C>>(cachedAssets_[filePath]);
		}

		template<typename C>
		static bool SaveAsset(CWString filePath, CAsset<C>& target)
		{
			SGReflectionStringOut strOut(target.getContentClass()->getReflection());
			return SaveToTextFile(filePath, strOut.out_);
		}

	protected:
		static CHashMap<CWString, std::shared_ptr<CAssetBase>> cachedAssets_;
	};
}
