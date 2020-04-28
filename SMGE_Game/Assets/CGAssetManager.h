#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "CGAsset.h"

namespace MonoMaxGraphics
{
	class CGAssetManager
	{
	public:
		static CSharPtr<CGAsset> FindAsset(CWString filePath);

		template<typename T>
		static CSharPtr<T> LoadAsset(CWString filePath);

		static CMap<CWString, CSharPtr<CGAsset>> allAssetMap_;
	};
}
