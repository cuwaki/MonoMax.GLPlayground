#include "CPointComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"
#include "CSphereComponent.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_PointComponent::SGRefl_PointComponent(TReflectionClass& rc) : Super(rc), sg_transform_(rc, rc), outerPointCompo_(rc)
	{
	}
	//SGRefl_PointComponent::SGRefl_PointComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_PointComponent(*uptr.get())
	//{
	//}

	void SGRefl_PointComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_PointComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		return ret;
	}

	SGReflection& SGRefl_PointComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		return *this;
	}

	CPointComponent::CPointComponent(CObject* outer) : CBoundComponent(outer)
	{
		//classRTTIName_ = "SMGE::CPointComponent";

		Ctor();
	}

	void CPointComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
		boundType_ = EBoundType::POINT;
	}

	void CPointComponent::OnBeginPlay(CObject* parent)
	{
		Super::OnBeginPlay(parent);
	}

	void CPointComponent::OnEndPlay()
	{
		Super::OnEndPlay();
	}

	SGReflection& CPointComponent::getReflection()
	{
		if (reflPointCompo_.get() == nullptr)
			reflPointCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflPointCompo_.get();
	}

	void CPointComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:point";

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if(rsm == nullptr)
			rsm = new nsRE::PointRSM();

		// ���� ���� - �̰� CResourceModel �� �����簡, ���ӿ������� �������� �ϵ��� ����
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(rsm));

		rsm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	bool CPointComponent::CheckCollide(CBoundComponent* checkTarget, glm::vec3& outCollidingPoint)
	{
		outCollidingPoint = nsRE::TransformConst::Vec3_Zero;
		return false;
	}
};
