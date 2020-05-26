#pragma once

#include "../GECommonIncludes.h"
#include "CInterfaceBase.h"
#include "../Components/CComponent.h"

namespace SMGE
{
	using ComponentVector = CVector<CUniqPtr<CComponent>>;

	class CInt_Component : public CInterfaceBase
	{
	public:
		virtual ComponentVector& getComponentList() = 0;
		virtual const ComponentVector& getComponentList() const { return getComponentList(); }
	};
};
