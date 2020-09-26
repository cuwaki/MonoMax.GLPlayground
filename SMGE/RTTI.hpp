#pragma once

#include <string>
#include <map>
#include <functional>

namespace SMGE
{
	template<class BaseClass, typename NewFunctor>
	class CRtti
	{
		using DelegateNewClass = std::function<NewFunctor>;

		using TFactory = std::map<std::string, DelegateNewClass>;
		static TFactory NewClassFactory;

	public:
		template<typename F>
		CRtti(std::string&& rttiName, F&& func)	// 일부러 정적인 입력들만 받기위하여 오른값들만 받음 - 즉 동적으로 작성 불가!
		{
			NewClassFactory[rttiName] = std::move(func);
		}

		static const DelegateNewClass& New(const std::string& className)
		{
			return NewClassFactory[className];
		}
	};
}
