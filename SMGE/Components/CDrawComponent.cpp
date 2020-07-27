#include "CDrawComponent.h"
#include "../Assets/CAssetManager.h"
#include "../Objects/CActor.h"

#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer), nsRE::WorldModel(nullptr)
	{
	}

	CDrawComponent::CDrawComponent(CObject* outer, const CWString& modelAssetPath) : CComponent(outer), nsRE::WorldModel(nullptr)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CDrawComponent::SetDrawingModelAsset(const CWString& modelAssetPath)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CDrawComponent::ReadyToDrawing()
	{
		if (drawingModelAsset_ != nullptr)
			throw SMGEException(wtext("already ReadyToDrawing("));

		// �� �ּ� �ε�
		drawingModelAsset_ = CAssetManager::LoadAsset<CAssetModel>(drawingModelAssetPath_);
		auto smgeMA = drawingModelAsset_->getContentClass();

		const nsRE::RenderingModel& rm = smgeMA->GetRenderingModel();
		rm.AddWorldModel(this);

		// ���� ����
		// ����𵨿� �θ� ���ͷκ����� Ʈ������ ó��
		//getTransform() = parentActor_->getWorldTransform();
		//myWorldModel_ = GetRenderingEngine()->AddWorldModel(new nsRE::OldModelWorld(*smgeMA));
		//myWorldModel_->modelMat = getTransform();
	}

	//class nsRE::CRenderingEngine* CDrawComponent::GetRenderingEngine()
	//{
	//	auto to = FindOuter<nsGE::CGameBase>(this);
	//	if (to != nullptr)
	//	{
	//		return to->GetEngine()->GetRenderingEngine();
	//	}

	//	return nullptr;
	//}

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
	}

	void CDrawComponent::OnEndPlay()
	{
		CComponent::OnEndPlay();
	}
};
