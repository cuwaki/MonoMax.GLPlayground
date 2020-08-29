#include "CSphereComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_SphereComponent::SGRefl_SphereComponent(TReflectionClass& rc) : Super(rc), radius_(rc.radius_), sg_transform_(rc, rc), outerSphereCompo_(rc)
	{
	}
	//SGRefl_SphereComponent::SGRefl_SphereComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_SphereComponent(*uptr.get())
	//{
	//}

	void SGRefl_SphereComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_SphereComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += _TO_REFL(float, radius_);

		return ret;
	}

	SGReflection& SGRefl_SphereComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(radius_, variableSplitted);

		return *this;
	}


	CSphereComponent::CSphereComponent(CObject* outer) : CBoundComponent(outer)
	{
		className_ = wtext("SMGE::CSphereComponent");

		Ctor();
	}

	void CSphereComponent::Ctor()
	{
		radius_ = 1.f;

		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
	}

	void CSphereComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CSphereComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CSphereComponent::getReflection()
	{
		if (reflSphereCompo_.get() == nullptr)
			reflSphereCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflSphereCompo_.get();
	}
};
