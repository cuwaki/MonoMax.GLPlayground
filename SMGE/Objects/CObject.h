#pragma once

#include "../GECommonIncludes.h"
#include "../RTTI.hpp"

namespace SMGE
{
	class CObject
	{
	public:
		CObject(CObject *outer);
		virtual ~CObject() noexcept {}

		CWString getClassName() const { return className_; }

		void Ctor();	// not virtual! ����μ��� �����ڳ� �ı��ڿ��� �Ҹ� �� �ֱ� ���� 20200813
		void Dtor();	// not virtual! ����μ��� �����ڳ� �ı��ڿ��� �Ҹ� �� �ֱ� ���� 20200813

		CObject* GetOuter() const;
		CObject* GetTopOuter();
		bool IsTopOuter();

	protected:
		CObject* outer_ = nullptr;
		CWString className_;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Template functions
	public:
		template<typename T>
		T* FindOuter(CObject* cur)
		{
			T* ret = nullptr;
			//T findingClass{ nullptr };	//if(IsA(outer, findingOuterClassName))	// ���� �۾� - CObject Ŭ������ static class name �ھƼ� template �Լ��� ã�� �� �ְ� ����

			if (DCast<T*>(cur) != nullptr)
				ret = SCast<T*>(cur);
			else
			{
				CObject* outer = cur->GetOuter();
				if (outer != nullptr)
				{
					ret = SCast<T*>(FindOuter<T>(outer));
				}
			}

			return ret;
		}
	};

	// CObject RTTI
	using RTTI_CObject = CRtti<CObject, CObject * (CObject*)>;

#define REGISTER_RTTI_CObject(className) RTTI_CObject _staticRTTI##className(""#className, [](CObject* outer) {return new className(outer); })
};
