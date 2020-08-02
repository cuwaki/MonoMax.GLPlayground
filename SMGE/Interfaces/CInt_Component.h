#pragma once

#include "../GECommonIncludes.h"
#include "CInterfaceBase.h"
#include "../Components/CComponent.h"

namespace SMGE
{
	using ComponentVector = CVector<CUniqPtr<CComponent>>;
	using ComponentVectorWeak = CVector<CComponent*>;

	class CInt_Component : public CInterfaceBase
	{
	public:
		virtual ComponentVector& getPersistentComponents() = 0;	// ����Ǵ� �͵�
		virtual ComponentVector& getTransientComponents() = 0;	// ������� �ʴ� �͵�

		virtual ComponentVectorWeak& getAllComponents() = 0;
		virtual const ComponentVectorWeak& getAllComponents() const { return getAllComponents(); }

		virtual void registerComponent(CComponent* weakCompo)
		{
			getAllComponents().push_back(weakCompo);
		}
		virtual bool unregisterComponent(CComponent* weakCompo)
		{
			auto it = SMGE::GlobalUtils::FindIt(getAllComponents(), weakCompo);
			return getAllComponents().erase(it) != getAllComponents().end();
		}

		virtual void clearAllComponents()
		{
			getPersistentComponents().clear();
			getTransientComponents().clear();
			getAllComponents().clear();
		}
	};
};
