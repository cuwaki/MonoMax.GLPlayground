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

		void Ctor();	// not virtual! 현재로서는 생성자나 파괴자에서 불릴 수 있기 때문 20200813
		void Dtor();	// not virtual! 현재로서는 생성자나 파괴자에서 불릴 수 있기 때문 20200813

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
			//T findingClass{ nullptr };	//if(IsA(outer, findingOuterClassName))	// 차후 작업 - CObject 클래스에 static class name 박아서 template 함수로 찾을 수 있게 하자

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
