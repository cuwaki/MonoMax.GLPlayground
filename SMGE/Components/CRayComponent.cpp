#include "CRayComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_RayComponent::SGRefl_RayComponent(TReflectionClass& rc) : Super(rc), size_(rc.size_), direction_(rc.direction_), sg_transform_(rc, rc), outerRayCompo_(rc)
	{
	}
	//SGRefl_RayComponent::SGRefl_RayComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_RayComponent(*uptr.get())
	//{
	//}

	void SGRefl_RayComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_RayComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += _TO_REFL(float, size_);
		ret += _TO_REFL(glm::vec3, direction_);

		return ret;
	}

	SGReflection& SGRefl_RayComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(size_, variableSplitted);
		_FROM_REFL(direction_, variableSplitted);

		return *this;
	}


	CRayComponent::CRayComponent(CObject* outer, float size, const glm::vec3& direction) : CBoundComponent(outer)
	{
		className_ = wtext("SMGE::CRayComponent");

		size_ = size;
		direction_ = direction;

		Ctor();
	}

	void CRayComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
	}

	void CRayComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CRayComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CRayComponent::getReflection()
	{
		if (reflRayCompo_.get() == nullptr)
			reflRayCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflRayCompo_.get();
	}

	void CRayComponent::ReadyToDrawing()
	{
		const auto resmKey = wtext("gizmoK:ray");

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if(rsm == nullptr)
			rsm = new nsRE::RayRSM(size_, direction_);

		// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(rsm));

		rsm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}
};
