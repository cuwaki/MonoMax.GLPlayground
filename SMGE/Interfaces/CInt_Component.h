#pragma once

#include "../GECommonIncludes.h"
#include "CInterfaceBase.h"
#include "../Components/CComponent.h"

namespace SMGE
{
	using ComponentVector = CVector<CUniqPtr<CComponent>>;
	using ComponentVectorWeak = CVector<CComponent*>;

	/*
		CInt_Component 상속시 해야할 일

		인터페이스의 순수가상함수들을 구현한다
		리플렉션을 구현한다
		Tick 에서 자식 콤포넌트의 틱을 돌려준다
		BeginPlay, EndPlay 에서 등록/해제 및 자식 콤포넌트들에게 전파한다
		필요시 - Render 에서 자식 콤포넌트의 렌더를 돌려준다

		자세한 예는 CDrawComponent 를 보면 된다
	*/

	class CInt_Component : public CInterfaceBase
	{
	public:
		virtual ComponentVector& getPersistentComponents() = 0;	// 저장되는 것들
		virtual ComponentVector& getTransientComponents() = 0;	// 저장되지 않는 것들
		virtual ComponentVectorWeak& getAllComponents() = 0;
		virtual const ComponentVectorWeak& getAllComponents() const { return getAllComponents(); }

		virtual void registerComponent(CComponent* weakCompo)
		{
			getAllComponents().push_back(weakCompo);
		}
		virtual bool unregisterComponent(CComponent* weakCompo)
		{
			auto it = SMGE::GlobalUtils::FindIt(getAllComponents(), weakCompo);
			if (it != getAllComponents().end())
			{
				getAllComponents().erase(it);
				return true;
			}

			return false;
		}
		virtual void unregisterComponentAll()
		{
			getAllComponents().clear();
		}
		virtual void clearAllComponents()
		{
			getPersistentComponents().clear();
			getTransientComponents().clear();
			getAllComponents().clear();
		}

		template<typename T, class CheckFunc>
		T* findComponent(CheckFunc& checkFunc)
		{
			auto found = std::find_if(getAllComponents().cbegin(), getAllComponents().cend(), [&checkFunc](auto& compo)
				{
					if (checkFunc(compo))
						return true;
					return false;
				});

			if (found != getAllComponents().cend())
				return DCast<T*>(*found);

			// 아직 비긴 플레이가 불리지 않았다면 밑으로 내려가게 될 것이다

			// 2. persistent
			const auto foundPers = std::find_if(getPersistentComponents().cbegin(), getPersistentComponents().cend(), [&checkFunc](auto& compo)
				{
					if (checkFunc(compo.get()))
						return true;
					return false;
				});

			if (foundPers != getPersistentComponents().cend())
				return DCast<T*>((*foundPers).get());

			// 3. transient
			const auto foundTrans = std::find_if(getTransientComponents().cbegin(), getTransientComponents().cend(), [&checkFunc](auto& compo)
				{
					if (checkFunc(compo.get()))
						return true;
					return false;
				});

			if (foundTrans != getTransientComponents().cend())
				return DCast<T*>((*foundTrans).get());

			return nullptr;
		}
	};
};
