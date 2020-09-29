#pragma once

#include <string>
#include <map>
#include <functional>
#include <any>

namespace SMGE
{
	struct CRttiNewFunctorVarietyBase
	{
	};

	template<class BaseClass, class TargetClass, typename... Args>
	struct CRttiNewFunctorVariety : public CRttiNewFunctorVarietyBase
	{
		BaseClass* operator()(Args&&... args)
		{
			return new TargetClass(std::forward<Args>(args)...);
		}
	};

	template<class BaseClass, typename NewFunctor>
	class CRtti
	{
		using NewFunctorDefaultT = std::map<std::string, std::function<NewFunctor>>;
		using NewFunctorVarietyT = std::map<std::string, CRttiNewFunctorVarietyBase>;

		static NewFunctorDefaultT NewClassDefaults_;
		static NewFunctorVarietyT NewClassVarieties_;

	public:
		CRtti(std::string&& rttiName, std::function<NewFunctor>&& func)	// 일부러 정적인 입력들만 받기위하여 오른값들만 받음 - 즉 동적으로 작성 불가!
		{
			NewClassDefaults_[rttiName] = std::move(func);
		}

		CRtti(std::string&& rttiName, CRttiNewFunctorVarietyBase&& func)
		{
			NewClassVarieties_[rttiName] = func;
		}

		template<typename... Args>
		static BaseClass* NewDefault(const std::string& classRTTIName, Args&&... args)
		{
			return NewClassDefaults_[classRTTIName](std::forward<Args>(args)...);
		}

		template<class TargetClass, typename... Args>
		static BaseClass* NewVariety(const std::string& classRTTIName, Args&&... args)
		{
			using castT = CRttiNewFunctorVariety<BaseClass, TargetClass, Args...>;
			return static_cast<castT&>(NewClassVarieties_[classRTTIName])(std::forward<Args>(args)...);
		}
	};
}
