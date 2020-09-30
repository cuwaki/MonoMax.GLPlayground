#include "CObject.h"

namespace SMGE
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CObject::CObject(CObject* outer) : outer_(outer)
	{
		//classRTTIName_ = "SMGE::CObject";
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
