#include "CGStaticMeshAsset.h"

SGRefl_StaticMeshAsset::SGRefl_StaticMeshAsset(const CGStaticMeshAsset& asset) : SGRefl_Asset(asset)
{
}

SGReflection& CGStaticMeshAsset::getReflection()
{
	if (reflAsset_ == false)
		reflAsset_ = MakeUniqPtr<ReflectionStruct>(*this);
	return *reflAsset_.get();
}
