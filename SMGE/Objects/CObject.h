#pragma once

#include "../GECommonIncludes.h"
#include "../RTTI.hpp"

namespace SMGE
{
	namespace nsGE
	{
		class CEngineBase;
	}
	namespace nsRE
	{
		class CRenderingEngine;
	}

	class CObject
	{
	public:
		CObject(CObject *outer);
		virtual ~CObject() noexcept {}

		void Ctor();	// not virtual! 현재로서는 생성자나 파괴자에서 불릴 수 있기 때문 20200813
		void Dtor();	// not virtual! 현재로서는 생성자나 파괴자에서 불릴 수 있기 때문 20200813

		CObject* GetOuter() const;
		CObject* GetTopOuter();
		bool IsTopOuter() const;

		const CString& GetCObjectTag() const { return cobjectTag_; }
		void SetCObjectTag(const CString& ot) { cobjectTag_ = ot; }

		class nsGE::CEngineBase* GetEngine() const;
		class nsRE::CRenderingEngine* GetRenderingEngine() const;

	protected:
		CObject* outer_ = nullptr;
		CString cobjectTag_;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Template functions
	public:
		template<typename T>
		const T* FindOuter(const CObject* cur) const
		{
			const T* ret = nullptr;
			//T findingClass{ nullptr };	//if(IsA(outer, findingOuterClassName))	// 차후 작업 - CObject 클래스에 static class name 박아서 template 함수로 찾을 수 있게 하자

			if (DCast<const T*>(cur) != nullptr)
				ret = SCast<const T*>(cur);
			else
			{
				const CObject* outer = cur->GetOuter();
				if (outer != nullptr)
				{
					ret = FindOuter<T>(outer);
				}
			}

			return ret;
		}

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
	/* 클래스 인스턴스용 */const std::string& GetClassRTTIName() const { return ClassRTTIName; }\
private:\
	/* private 이어야한다 - 자식들도 함부로 바꾸면 안됨 */static CString ClassRTTIName;

#define DEFINE_RTTI_CObject_DEFAULT(CRN) CString CRN::ClassRTTIName = ""#CRN; RTTI_CObject _staticRTTI_DEFAULT_##CRN(""#CRN, [](CObject* outer) {return new CRN(outer); }); 
#define DEFINE_RTTI_CObject_VARIETY(CRN, ...) RTTI_CObject _staticRTTI_VARIETY_##CRN(""#CRN, CRttiNewFunctorVariety<CObject, CRN, __VA_ARGS__>{});
};
