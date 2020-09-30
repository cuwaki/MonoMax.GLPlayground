#include "CCubeComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_CubeComponent::SGRefl_CubeComponent(TReflectionClass& rc) : Super(rc), centerPos_(rc.centerPos_), size_(rc.size_), sg_transform_(rc, rc), outerCubeCompo_(rc)
	{
	}
	//SGRefl_CubeComponent::SGRefl_CubeComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_CubeComponent(*uptr.get())
	//{
	//}

	void SGRefl_CubeComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_CubeComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += _TO_REFL(glm::vec3, centerPos_);
		ret += _TO_REFL(glm::vec3, size_);

		return ret;
	}

	SGReflection& SGRefl_CubeComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(centerPos_, variableSplitted);
		_FROM_REFL(size_, variableSplitted);

		return *this;
	}

	CCubeComponent::CCubeComponent(CObject* outer, const glm::vec3& leftBottom, const glm::vec3& rightTop) : CBoundComponent(outer)
	{
		//classRTTIName_ = "SMGE::CCubeComponent";
		Ctor();
	}

	void CCubeComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::RAY;
	}

	void CCubeComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CCubeComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CCubeComponent::getReflection()
	{
		if (reflCubeCompo_.get() == nullptr)
			reflCubeCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflCubeCompo_.get();
	}

	void CCubeComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:cube";

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if (rsm == nullptr)
			rsm = new nsRE::CubeRSM(centerPos_, size_);

		// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(rsm));

		rsm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CCubeComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		return false;
	}
};
