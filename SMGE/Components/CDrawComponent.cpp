#include "CDrawComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Transform::SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans) : SGReflection(anything), nsre_transform_(trans)
	{
		className_ = wtext("SMGE::SGRefl_Transform");
	}

	SGRefl_Transform::operator CWString() const
	{
		CWString ret = Super::operator SMGE::CWString();

		ret += _TO_REFL(glm::vec3, nsre_transform_.GetTranslation());
		ret += _TO_REFL(glm::vec3, nsre_transform_.GetRotation());
		ret += _TO_REFL(glm::vec3, nsre_transform_.GetScale());

		return ret;
	}

	SGReflection& SGRefl_Transform::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		glm::vec3 translation, rotation, scale;
		_FROM_REFL(translation, variableSplitted);
		_FROM_REFL(rotation, variableSplitted);
		_FROM_REFL(scale, variableSplitted);

		nsre_transform_.Translate(translation);
		nsre_transform_.Rotate(rotation);
		nsre_transform_.Scale(scale);

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_DrawComponent::SGRefl_DrawComponent(TReflectionClass& rc) : Super(rc), sg_transform_(rc.sg_drawTransform_)
	{
	}
	SGRefl_DrawComponent::SGRefl_DrawComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_DrawComponent(*uptr.get())
	{
	}

	SGRefl_DrawComponent::operator CWString() const
	{
		auto ret = Super::operator CWString();

		ret += static_cast<CWString>(sg_transform_);

		return ret;
	}

	SGReflection& SGRefl_DrawComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		sg_transform_ = variableSplitted;

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer), nsRE::WorldModel(nullptr), sg_drawTransform_(*this, *this)
	{
		className_ = wtext("SMGE::CDrawComponent");
	}

	void CDrawComponent::ReadyToDrawing()
	{
	}

	class nsRE::CRenderingEngine* CDrawComponent::GetRenderingEngine()
	{
		auto to = FindOuter<nsGE::CGameBase>(this);
		if (to != nullptr)
		{
			return to->GetEngine()->GetRenderingEngine();
		}

		return nullptr;
	}

	void CDrawComponent::Tick(float td)
	{
	}

	void CDrawComponent::Render(float td)
	{
	}

	void CDrawComponent::OnBeginPlay(CActor* parent)
	{
		CComponent::OnBeginPlay(parent);

		ReadyToDrawing();

		// 여기 수정
		// 월드모델에 부모 액터로부터의 트랜스폼 처리
		//getTransform() = parentActor_->getWorldTransform();
		//myWorldModel_ = GetRenderingEngine()->AddWorldModel(new nsRE::OldModelWorld(*smgeMA));
		//myWorldModel_->modelMat = getTransform();
	}

	void CDrawComponent::OnEndPlay()
	{
		CComponent::OnEndPlay();
	}
};
