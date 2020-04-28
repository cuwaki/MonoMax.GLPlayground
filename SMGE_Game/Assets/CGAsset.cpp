#include "CGAsset.h"

namespace MonoMaxGraphics
{
	SGRefl_Asset::SGRefl_Asset(const CGAsset& asset) :
		filePath_(asset.filePath_),
		SGReflection(asset)
	{
	}

	//SGReflection& CGAsset::getReflection()
	//{
	//	if(reflAsset_ == false)
	//		reflAsset_ = MakeUniqPtr<SGRefl_Asset>(*this);
	//	return *reflAsset_.get();
	//}
};
