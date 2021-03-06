#include "CDrawComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"

#include "CBoundComponent.h"

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Transform::SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans) : SGReflection(anything), nsre_transform_(trans)
	{
	}

	const SGReflection& SGRefl_Transform::operator>>(CWString& out) const
	{
		Super::operator>>(out);

		// 펜딩이 맞다 - 각자 자신의 트랜스폼을 저장하므로
		out += _TO_REFL(glm::vec3, nsre_transform_.GetPendingPosition());
		const auto degRot = nsre_transform_.GetPendingRotationEulerDegreesWorld();
		out += _TO_REFL(glm::vec3, degRot);
		out += _TO_REFL(glm::vec3, nsre_transform_.GetPendingScales());
		out += _TO_REFL(glm::vec3, nsre_transform_.GetAnchor());

		return *this;
	}

	SGReflection& SGRefl_Transform::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		glm::vec3 translation, rotation, scale, anchor;
		_FROM_REFL(translation, in);
		_FROM_REFL(rotation, in);
		_FROM_REFL(scale, in);
		_FROM_REFL(anchor, in);

		// 펜딩이 맞다 - 각자 자신의 트랜스폼을 저장하므로
		nsre_transform_.Translate(translation);
#ifdef REFACTORING_TRNASFORM
		nsre_transform_.RotateEuler(rotation, true);	// 최초 모델의 기준 축 설정 - 월드로 반영
#else
		nsre_transform_.RotateEuler(rotation);
#endif
		nsre_transform_.Scale(scale);
		nsre_transform_.SetAnchor(anchor);

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_DrawComponent::SGRefl_DrawComponent(TReflectionClass& rc) : Super(rc), sg_transform_(rc, rc), outerDrawCompo_(rc)
	{
	}
	//SGRefl_DrawComponent::SGRefl_DrawComponent(const UPtr<CDrawComponent>& uptr) : SGRefl_DrawComponent(*uptr.get())
	//{
	//}

	const SGReflection& SGRefl_DrawComponent::operator>>(CWString& out) const
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
		Super::operator>>(out);
		
		sg_transform_ >> out;

		const auto persistCompoSize = outerDrawCompo_.getPersistentComponents().size();
		out += _TO_REFL(size_t, persistCompoSize);

		auto& pcomps = outerDrawCompo_.getPersistentComponents();
		for (int i = 0; i < pcomps.size(); ++i)
		{
			pcomps[i]->getReflection() >> out;
		}

		return *this;
	}

	SGReflection& SGRefl_DrawComponent::operator<<(const CVector<TupleVarName_VarType_Value>& in)
	{
		Super::operator<<(in);

		sg_transform_ << in;

		size_t persistCompoSize = 0;
		_FROM_REFL(persistCompoSize, in);

		auto& pcomps = outerDrawCompo_.getPersistentComponents();
		pcomps.clear();
		pcomps.reserve(persistCompoSize);

		for (size_t i = 0; i < persistCompoSize; ++i)
		{
			auto compo = ReflectionUtils::FuncLoadClass<CComponent>(&outerDrawCompo_, in);
			if (compo != nullptr)
				pcomps.emplace_back(std::move(compo));
		}

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer), nsRE::WorldModel(nullptr)
	{
		Ctor();
	}

	void CDrawComponent::Ctor()
	{
		isGameRendering_ = false;
#if IS_EDITOR
		isEditorRendering_ = false;
#endif
	}

	void CDrawComponent::ReadyToDrawing()
	{
	}

	void CDrawComponent::Tick(float td)
	{
		Super::Tick(td);

		for (auto& comp : getAllComponents())
		{
			comp->Tick(td);
		}
	}

	void CDrawComponent::OnBeginPlay(CObject* parent)
	{
		assert(parent != nullptr);

		CComponent::OnBeginPlay(parent);

		if (isGameRendering_)
			SetRendering(isGameRendering_, true);

#if IS_EDITOR
		if (isEditorRendering_)	// 게임일 경우 에디터 비지블이 게임 비지블을 덮어써버리도록 하자
			SetRendering(isEditorRendering_, true);
#endif

		ReadyToDrawing();

		nsRE::Transform* parentTransf = nullptr;

		// 여기 수정 - 흠.... 이거 어떻게?
		CActor* actorParent = dynamic_cast<CActor*>(parent);
		if(actorParent)
			parentTransf = &actorParent->getTransform();
		else
			parentTransf = dynamic_cast<nsRE::Transform*>(parent);

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

	bool CDrawComponent::IsGameRendering() const
	{
		return isGameRendering_;
	}
	void CDrawComponent::SetGameRendering(bool ir)
	{
		isGameRendering_ = ir;
		SetRendering(ir, true);
	}

#if IS_EDITOR
	bool CDrawComponent::IsEditorRendering() const
	{
		return isEditorRendering_;
	}

	void CDrawComponent::SetEditorRendering(bool ir)
	{
		isEditorRendering_ = ir;
		SetRendering(ir, true);
	}
#endif

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

	class CBoundComponent* CDrawComponent::GetMainBound()
	{
		// 여기 - 최적화 - 콤포넌트의 변화가 있을 때만 재계산이 되어야한다
		if (mainBoundCompo_ == nullptr)
		{	// 여기 - transient 를 메인으로 삼은 경우 문제가 될 수 있는 점이 개선되어야한다
			mainBoundCompo_ = findComponent<CBoundComponent>([](auto compoPtr)
				{
					auto bc = dynamic_cast<CBoundComponent*>(compoPtr);
					if (bc && bc->IsCollideTarget())
						return true;
					return false;
				});
		}

		return mainBoundCompo_;
	}
};
