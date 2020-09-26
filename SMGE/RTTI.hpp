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
		CRtti(std::string&& rttiName, F&& func)	// �Ϻη� ������ �Էµ鸸 �ޱ����Ͽ� �������鸸 ���� - �� �������� �ۼ� �Ұ�!
		{
			NewClassFactory[rttiName] = std::move(func);
		}

		static const DelegateNewClass& New(const std::string& className)
		{
			return NewClassFactory[className];
		}
	};
}
