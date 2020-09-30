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

		void Ctor();	// not virtual! ����μ��� �����ڳ� �ı��ڿ��� �Ҹ� �� �ֱ� ���� 20200813
		void Dtor();	// not virtual! ����μ��� �����ڳ� �ı��ڿ��� �Ҹ� �� �ֱ� ���� 20200813

		CObject* GetOuter() const;
		CObject* GetTopOuter();
		bool IsTopOuter();

	protected:
		CObject* outer_ = nullptr;

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
					ret = FindOuter<T>(outer);
				}
			}

			return ret;
		}
	};

	// CObject RTTI
	using RTTI_CObject = CRtti<CObject, CObject * (CObject*)>;

#define DECLARE_RTTI_CObject(CRN)\
public :\
	/* Ŭ���� �ν��Ͻ��� */const std::string& GetClassRTTIName() const { return ClassRTTIName; }\
private:\
	/* private �̾���Ѵ� - �ڽĵ鵵 �Ժη� �ٲٸ� �ȵ� */static CString ClassRTTIName;

#define DEFINE_RTTI_CObject_DEFAULT(CRN) CString CRN::ClassRTTIName = ""#CRN; RTTI_CObject _staticRTTI_DEFAULT_##CRN(""#CRN, [](CObject* outer) {return new CRN(outer); }); 
#define DEFINE_RTTI_CObject_VARIETY(CRN, ...) RTTI_CObject _staticRTTI_VARIETY_##CRN(""#CRN, CRttiNewFunctorVariety<CObject, CRN, __VA_ARGS__>{});

	/* static Ŭ������ */
	template<class T>
	const std::string& GetClassRTTIName()
	{
		return T(nullptr).GetClassRTTIName();
	}
};
