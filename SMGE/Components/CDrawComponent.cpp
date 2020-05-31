#include "CDrawComponent.h"
#include "../Assets/CAssetManager.h"
#include "../Objects/CActor.h"

#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	CDrawComponent::CDrawComponent(CObject *outer) : CComponent(outer)
	{
		objectTransform_ = glm::mat4(1.f);
		objectLocation_ = glm::vec3(0.f);
		objectDirection_ = glm::vec3(1.f, 0.f, 0.f);
		objectScale_ = glm::vec3(1.f, 1.f, 1.f);
	}

	CDrawComponent::CDrawComponent(CObject* outer, const CWString& modelAssetPath) : CComponent(outer)
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

		// 모델 애셋 로드
		drawingModelAsset_ = CAssetManager::LoadAsset<CModelData>(drawingModelAssetPath_);
		auto smgeMA = drawingModelAsset_->getContentClass();

		// 월드 모델 생성
		getTransform() = parentActor_->getWorldTransform();
		myWorldModel_ = GetRenderingEngine()->AddWorldModel(new nsRE::WorldModel(*smgeMA));
		myWorldModel_->modelMat = getTransform();
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
	}

	void CDrawComponent::OnEndPlay()
	{
		CComponent::OnEndPlay();

		if (myWorldModel_ != nullptr)
		{
			GetRenderingEngine()->RemoveWorldModel(myWorldModel_);
			myWorldModel_ = nullptr;
		}
	}

	glm::mat4& CDrawComponent::getTransform()
	{
		return objectTransform_;
	}
	glm::vec3& CDrawComponent::getLocation()
	{
		return objectLocation_;
	}
	glm::vec3& CDrawComponent::getDirection()
	{
		return objectDirection_;
	}
	glm::vec3& CDrawComponent::getScale()
	{
		return objectScale_;
	}
};
