#include "CDrawComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Transform::SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans) : SGReflection(anything), nsre_transform_(trans)
	{
	}

	SGRefl_Transform::operator CWString() const
	{
		CWString ret = Super::operator CWString();

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
	SGRefl_DrawComponent::SGRefl_DrawComponent(TReflectionClass& rc) : Super(rc), sg_transform_(rc, rc), outerDrawCompo_(rc)
	{
	}
	//SGRefl_DrawComponent::SGRefl_DrawComponent(const CUniqPtr<CDrawComponent>& uptr) : SGRefl_DrawComponent(*uptr.get())
	//{
	//}

	void SGRefl_DrawComponent::OnBeforeSerialize() const
	{
		Super::OnBeforeSerialize();
	}

	SGRefl_DrawComponent::operator CWString() const
	{
		/* 여기 수정 - 여기 호출하면 
				className_$CWString$"SMGE::SGRefl_Transform"
					reflectionFilePath_$CWString$""

				이 아니라

			className_$CWString$"SMGE::CMeshComponent"
				reflectionFilePath_$CWString$""

				로 나가서 뻑난다

				이거 왜 이렇게 될ㄲㅏ?

			여기 때문인 것 같은데?? $$43

			지금 여기 말고도 아래처럼 자식 클래스를 CWString 으로 캐스팅하는 부분이 있는데 다른 클래스들에서도 문제가 생기고 있다 className_ 과 reflectionName_ 의 초기화를 점검해봐야겠다!!
			단 실행에는 현재로써는 지장이 없는 상태기는 하다 20200817
		*/
		auto ret = Super::operator CWString();
		
		ret += SCast<CWString>(sg_transform_);

		const auto persistCompoSize = outerDrawCompo_.getPersistentComponents().size();
		ret += _TO_REFL(size_t, persistCompoSize);

		auto& pcomps = outerDrawCompo_.getPersistentComponents();
		for (int i = 0; i < pcomps.size(); ++i)
		{
			ret += SCast<CWString>(pcomps[i]->getReflection());
		}

		return ret;
	}

	SGReflection& SGRefl_DrawComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		sg_transform_ = variableSplitted;

		size_t persistCompoSize = 0;
		_FROM_REFL(persistCompoSize, variableSplitted);

		auto& pcomps = outerDrawCompo_.getPersistentComponents();
		pcomps.clear();
		pcomps.reserve(persistCompoSize);

		for (size_t i = 0; i < persistCompoSize; ++i)
		{
			auto compo = ReflectionUtils::FuncLoadClass<CComponent>(&outerDrawCompo_, variableSplitted);
			if (compo != nullptr)
				pcomps.emplace_back(std::move(compo));
		}

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer), nsRE::WorldObject(nullptr)
	{
		Ctor();
	}

	void CDrawComponent::Ctor()
	{
		isGameVisible_ = false;
#if IS_EDITOR
		isEditorVisible_ = false;
#endif
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
		Super::Tick(td);

		for (auto& comp : getAllComponents())
		{
			comp->Tick(td);
		}
	}

	void CDrawComponent::Render(float td)
	{
		for (auto& comp : getAllComponents())
		{
			auto mc = DCast<CDrawComponent*>(comp);
			if (mc)
				mc->Render(td);
		}
	}

	void CDrawComponent::OnBeginPlay(CObject* parent)
	{
		assert(parent != nullptr);

		CComponent::OnBeginPlay(parent);

		if (isGameVisible_)
			SetVisible(isGameVisible_);

#if IS_EDITOR
		if (isEditorVisible_)	// 게임일 경우 에디터 비지블이 게임 비지블을 덮어써버리도록 하자
			SetVisible(isEditorVisible_);
#endif

		ReadyToDrawing();

		nsRE::Transform* parentTransf = nullptr;

		// 여기 수정 - 흠.... 이거 어떻게?
		CActor* actorParent = DCast<CActor*>(parent);
		if(actorParent)
			parentTransf = &actorParent->getTransform();
		else
			parentTransf = DCast<nsRE::Transform*>(parent);

		ChangeParent(parentTransf);

		for (auto& pc : getPersistentComponents())
		{
			registerComponent(pc.get());
		}

		for (auto& pc : getTransientComponents())
		{
			registerComponent(pc.get());
		}

		for (auto& comp : getAllComponents())
		{
			comp->OnBeginPlay(this);
		}
	}

	void CDrawComponent::OnEndPlay()
	{
		CComponent::OnEndPlay();

		for (auto& comp : getAllComponents())
		{
			comp->OnEndPlay();
		}

		for (auto& pc : getPersistentComponents())
		{
			unregisterComponent(pc.get());
		}

		for (auto& pc : getTransientComponents())
		{
			unregisterComponent(pc.get());
		}
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
