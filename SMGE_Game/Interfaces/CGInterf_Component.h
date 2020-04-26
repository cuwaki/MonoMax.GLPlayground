#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"

#include "../Components/CGComponent.h"

class CGInterf_Component : public CGInterface
{
public:
	virtual CVector<CUniqPtr<CGComponent>>& getComponentList() = 0;
	virtual const CVector<CUniqPtr<CGComponent>>& getComponentList() const { return getComponentList(); }
};
