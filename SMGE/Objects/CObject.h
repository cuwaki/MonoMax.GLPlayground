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

	public:
		static CString ClassRTTIName;	// DECLARE_RTTI_CObject

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

#define DECLARE_RTTI_CObject(classRTTIName) public : static CString ClassRTTIName; private:

#define DEFINE_RTTI_CObject_DEFAULT(classRTTIName) CString classRTTIName::ClassRTTIName = ""#classRTTIName; RTTI_CObject _staticRTTI_DEFAULT_##classRTTIName(""#classRTTIName, [](CObject* outer) {return new classRTTIName(outer); }); 
#define DEFINE_RTTI_CObject_VARIETY(classRTTIName, ...) RTTI_CObject _staticRTTI_VARIETY_##classRTTIName(""#classRTTIName, CRttiNewFunctorVariety<CObject, classRTTIName, __VA_ARGS__>{});
};
