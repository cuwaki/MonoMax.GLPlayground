#include "CMeshComponent.h"
#include "../Assets/CAssetManager.h"
#include "../Objects/CActor.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	SGRefl_MeshComponent::SGRefl_MeshComponent(TReflectionClass& meshc) : Super(meshc), 
		drawingModelAssetPath_(meshc.drawingModelAssetPath_)
	{
	}
	//SGRefl_MeshComponent::SGRefl_MeshComponent(const CUniqPtr<CMeshComponent>& uptr) : SGRefl_MeshComponent(*uptr.get())
	//{
	//}

	SGRefl_MeshComponent::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(CWString, drawingModelAssetPath_);
		return ret;
	}

	SGReflection& SGRefl_MeshComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(drawingModelAssetPath_, variableSplitted);
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CMeshComponent::CMeshComponent(CObject* outer) : CDrawComponent(outer)
	{
		className_ = wtext("SMGE::CMeshComponent");
	}

	CMeshComponent::CMeshComponent(CObject* outer, const CWString& modelAssetPath) : CMeshComponent(outer)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	CMeshComponent::~CMeshComponent()
	{
		if (drawingModelAsset_ != nullptr)
		{	// ���� ���� - �̰� CAssetModel �� �����簡, ���ӿ������� �������� �ϵ��� ����
			auto smgeMA = drawingModelAsset_->getContentClass();
			GetRenderingEngine()->RemoveAssetModel(smgeMA);
		}
	}

	void CMeshComponent::SetDrawingModelAsset(const CWString& modelAssetPath)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CMeshComponent::ReadyToDrawing()
	{
		if (drawingModelAsset_ != nullptr)
			throw SMGEException(wtext("already ReadyToDrawing("));

		// �� �ּ� �ε�
		auto rootAssetPath = nsGE::CGameBase::Instance->PathAssetRoot();
		drawingModelAsset_ = CAssetManager::LoadAsset<CAssetModel>(rootAssetPath + drawingModelAssetPath_);
		auto smgeMA = drawingModelAsset_->getContentClass();

		// ���� ���� - �̰� CAssetModel �� �����簡, ���ӿ������� �������� �ϵ��� ����
		GetRenderingEngine()->AddAssetModel(drawingModelAssetPath_, smgeMA);

		const nsRE::RenderingModel& rm = smgeMA->GetRenderingModel();
		rm.AddWorldModel(this);

		Super::ReadyToDrawing();
	}

	SGReflection& CMeshComponent::getReflection()
	{
		if (reflMeshCompo_.get() == nullptr)
			reflMeshCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflMeshCompo_.get();
	}
};
