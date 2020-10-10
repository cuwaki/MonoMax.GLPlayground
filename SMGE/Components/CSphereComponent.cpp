#include "CSphereComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "../../MonoMax.EngineCore/RenderingEngineGizmo.h"

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
		Ctor();
	}

	void CSphereComponent::Ctor()
	{
		radius_ = 1.f;

		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif

		boundType_ = EBoundType::SPHERE;
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

	void CSphereComponent::ReadyToDrawing()
	{
		const auto resmKey = "gizmoK:sphere";

		auto rsm = GetRenderingEngine()->GetResourceModel(resmKey);
		if (rsm == nullptr)
			rsm = new nsRE::SphereRSM(radius_);

		// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
		GetRenderingEngine()->AddResourceModel(resmKey, std::move(rsm));

		rsm->GetRenderModel().AddWorldObject(this);

		Super::ReadyToDrawing();
	}

	class CCubeComponent* CSphereComponent::GetOBB()
	{
		if (cachedOBB_ == nullptr)
		{
			auto centerPos = glm::vec3(0.f);// 모델 좌표계에 생성해야한다, 부모의 transform을 따라야하기 때문
			cachedOBB_ = CreateOBB(centerPos - radius_, centerPos + radius_);
		}

		return cachedOBB_;
	}
};
