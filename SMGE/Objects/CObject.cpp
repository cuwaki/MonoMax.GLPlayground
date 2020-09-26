#include "CObject.h"

namespace SMGE
{
	// CObject RTTI 
	RTTI_CObject::TFactory RTTI_CObject::NewClassFactory;
	REGISTER_RTTI_CObject(CObject);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CObject::CObject(CObject* outer) : outer_(outer)
	{
		className_ = wtext("SMGE::CObject");
	}

	void CObject::Ctor()
	{
	}
	
	void CObject::Dtor()
	{
	}

	CObject* CObject::GetOuter() const
	{
		return outer_;
	}

	CObject* CObject::GetTopOuter()
	{
		CObject* outer = GetOuter();
		if (outer != nullptr)
			return outer->GetTopOuter();

		return this;
	}

	bool CObject::IsTopOuter()
	{
		return outer_ == nullptr;
	}
}
