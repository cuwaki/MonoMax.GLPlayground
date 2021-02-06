#include "CObject.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CObject::CObject(CObject* outer) : outer_(outer)
	{
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

	bool CObject::IsTopOuter() const
	{
		return outer_ == nullptr;
	}

	class CEngineBase* CObject::GetEngine() const
	{
		const auto to = FindOuter<CGameBase>(this);
		if (to != nullptr)
		{
			return to->GetEngine();
		}

		return nullptr;
	}

	class nsRE::CRenderingEngine* CObject::GetRenderingEngine() const
	{
		const auto to = FindOuter<CGameBase>(this);
		if (to != nullptr)
		{
			return to->GetEngine()->GetRenderingEngine();
		}

		return nullptr;
	}
}
