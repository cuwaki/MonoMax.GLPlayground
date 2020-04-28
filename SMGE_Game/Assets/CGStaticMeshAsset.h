#pragma once

#include "CGAsset.h"

namespace MonoMaxGraphics
{
	class CGStaticMeshAsset;

	struct SGRefl_StaticMeshAsset : public SGRefl_Asset
	{
		SGRefl_StaticMeshAsset(const CGStaticMeshAsset& asset);
	};

	class CGStaticMeshAsset : public CGAsset
	{
		using ReflectionStruct = SGRefl_StaticMeshAsset;
		friend struct ReflectionStruct;

	public:
		virtual SGReflection& getReflection() override;
	};
};
