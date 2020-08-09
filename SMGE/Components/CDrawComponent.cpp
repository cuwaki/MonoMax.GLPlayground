#include "CDrawComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Transform::SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans) : SGReflection(anything), nsre_transform_(trans)
	{
		reflectionName_ = wtext("SMGE::SGRefl_Transform");
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
	//SGRefl_DrawComponent::SGRefl_DrawComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_DrawComponent(*uptr.get())
	//{
	//}

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
		assert(parent != nullptr);

		CComponent::OnBeginPlay(parent);

		ReadyToDrawing();

		// 부모 액터로의 트랜스폼 연결
		ChangeParent(&parent->getTransform());

		//this 의 트랜스폼은 부모 액터로부터의 상대 트랜스폼이다
		//부모가 바뀌면 나도 바뀐다
		//	내가 바뀔 때 자식들에게 영향을 미쳐야한다
		//		이 영향의 계산이 즉시? 지연? 어떻게 되어야하나?

		//트랜스폼이 더티되면 그려지기 전에 갱신된다
		//	부모트랜스폼이 더티라면 나도 더티다
		//
		//자식이 그려지려면 부모가 그려져야한다
		//	
		//부모가 바뀌는 일이 생기기도 하므로 포인터로 해야한다 - 셰어드가 되어야겠네
	}

	void CDrawComponent::OnEndPlay()
	{
		CComponent::OnEndPlay();
	}


	ComponentVector& CDrawComponent::getPersistentComponents()
	{
		return persistentComponents_;
	}

	ComponentVector& CDrawComponent::getTransientComponents()
	{
		return transientComponents_;
	}

	ComponentVectorWeak& CDrawComponent::getAllComponents()
	{
		return allComponents_;
	}
};
