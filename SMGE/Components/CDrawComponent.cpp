#include "CDrawComponent.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../Objects/CActor.h"
#include "CMeshComponent.h"	// �׽�Ʈ �ڵ�

namespace SMGE
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SGRefl_Transform::SGRefl_Transform(CInt_Reflection& anything, nsRE::Transform& trans) : SGReflection(anything), nsre_transform_(trans)
	{
		reflectionName_ = wtext("SMGE::SGRefl_Transform");
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

		// { persistentComponentsREFL_ RTTI �ʿ� �̽�
		persistentComponentNumber_ = outerDrawCompo_.getPersistentComponents().size();
	}

	SGRefl_DrawComponent::operator CWString() const
	{
		/* ���� ���� - ���� ȣ���ϸ� 
				className_$CWString$"SMGE::SGRefl_Transform"
					reflectionFilePath_$CWString$""

				�� �ƴ϶�

			className_$CWString$"SMGE::CMeshComponent"
				reflectionFilePath_$CWString$""

				�� ������ ������

				�̰� �� �̷��� �ɤ���?

			���� ������ �� ������?? $$43

			���� ���� ���� �Ʒ�ó�� �ڽ� Ŭ������ CWString ���� ĳ�����ϴ� �κ��� �ִµ� �ٸ� Ŭ�����鿡���� ������ ����� �ִ� className_ �� reflectionName_ �� �ʱ�ȭ�� �����غ��߰ڴ�!!
			�� ���࿡�� ����ν�� ������ ���� ���±�� �ϴ� 20200817
		*/
		auto ret = Super::operator CWString();
		
		ret += SCast<CWString>(sg_transform_);

		// �׽�Ʈ �ڵ� �� ���� �κ��̴�
		ret += _TO_REFL(int32_t, persistentComponentNumber_);

		auto& pcomps = outerDrawCompo_.getPersistentComponents();

		assert(persistentComponentNumber_ == pcomps.size());

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

		// �׽�Ʈ �ڵ� �� �о���̴� �κ��̴�
		_FROM_REFL(persistentComponentNumber_, variableSplitted);
		auto& pcomps = outerDrawCompo_.getPersistentComponents();

		for (int i = 0; i < persistentComponentNumber_; ++i)
		{
			// �׽�Ʈ �ڵ� �� ����ν�� �޽������� �����Ѵ� - �̰� ����� ó���Ϸ��� RTTI �ʿ� �̽�
			auto meshComp = MakeUniqPtr<CMeshComponent>(&outerDrawCompo_);	// ���� ������ �� ������?? $$43 ���⼭ �ƿ��ͷ� outerDrawCompo_ �� �ָ鼭 ����� ���� ��...
			meshComp.get()->getReflection() = variableSplitted;
			pcomps.emplace_back(std::move(meshComp));
		}

		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer), nsRE::WorldObject(nullptr)
	{
		className_ = wtext("SMGE::CDrawComponent");

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
		if (isEditorVisible_)	// ������ ��� ������ �������� ���� �������� ������������ ����
			SetVisible(isEditorVisible_);
#endif

		ReadyToDrawing();

		nsRE::Transform* parentTransf = nullptr;

		// ���� ���� - ��.... �̰� ���?
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
