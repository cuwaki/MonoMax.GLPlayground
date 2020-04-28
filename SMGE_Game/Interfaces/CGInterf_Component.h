#pragma once

#include "../GCommonIncludes.h"
#include "CGInterface.h"
#include "../Components/CGComponent.h"

namespace MonoMaxGraphics
{
	using ComponentVector = CVector<CUniqPtr<CGComponent>>;

	class CGInterf_Component : public CGInterface
	{
	public:
		virtual ComponentVector& getComponentList() = 0;
		virtual const ComponentVector& getComponentList() const { return getComponentList(); }
	};
};
