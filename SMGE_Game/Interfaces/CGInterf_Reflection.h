#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"

struct SGReflection
{
	SGReflection() {}
};

class CGInterf_Reflection : public CGInterface
{
public:
	virtual SGReflection& getReflection() = 0;
	virtual const SGReflection& getReflection() const { return getReflection(); }
};
