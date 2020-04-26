#pragma once

#include "../../SMGE/GECommonIncludes.h"
#include "../Objects/CGObject.h"
#include "../Interfaces/CGInterf_Reflection.h"

class CGAsset;

struct SGRefl_Asset : public SGReflection
{
	SGRefl_Asset(const CGAsset& asset);

	CWString &filePath_;
};

class CGAsset : public CGObject, public CGInterf_Reflection
{
	using ReflectionStruct = SGRefl_Asset;
	friend struct ReflectionStruct;

public:
	virtual SGReflection& getReflection() = 0;

protected:
	CWString& filePath_;

	CUniqPtr<SGRefl_Asset> reflAsset_;
};
