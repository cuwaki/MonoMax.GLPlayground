#include "CGAsset.h"

SGRefl_Asset::SGRefl_Asset(const CGAsset& asset) :
	filePath_(asset.filePath_)
{
}

//SGReflection& CGAsset::getReflection()
//{
//	if(reflAsset_ == false)
//		reflAsset_ = MakeUniqPtr<SGRefl_Asset>(*this);
//	return *reflAsset_.get();
//}
