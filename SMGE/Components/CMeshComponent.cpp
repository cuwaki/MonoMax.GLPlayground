#include "CMeshComponent.h"
#include "../Assets/CAssetManager.h"
#include "../Objects/CActor.h"
#include "../CGameBase.h"
#include "../CEngineBase.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	SGRefl_MeshComponent::SGRefl_MeshComponent(TReflectionClass& meshc) : Super(meshc), 
		resourceModelAssetPath_(meshc.resourceModelAssetPath_)
	{
	}
	//SGRefl_MeshComponent::SGRefl_MeshComponent(const std::unique_ptr<CMeshComponent>& uptr) : SGRefl_MeshComponent(*uptr.get())
	//{
	//}

	SGRefl_MeshComponent::operator CWString() const
	{
		CWString ret = Super::operator CWString();

		ret += _TO_REFL(CWString, resourceModelAssetPath_);
		return ret;
	}

	SGReflection& SGRefl_MeshComponent::operator=(CVector<TupleVarName_VarType_Value>& variableSplitted)
	{
		Super::operator=(variableSplitted);

		_FROM_REFL(resourceModelAssetPath_, variableSplitted);
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CMeshComponent::CMeshComponent(CObject* outer) : CDrawComponent(outer)
	{
		Ctor();
	}

	CMeshComponent::CMeshComponent(CObject* outer, const CWString& modelAssetPath) : CMeshComponent(outer)
	{
		resourceModelAssetPath_ = modelAssetPath;
	}

	void CMeshComponent::Ctor()
	{
		isGameRendering_ = true;
#if IS_EDITOR
		isEditorRendering_ = true;
#endif
	}

	void CMeshComponent::SetDrawingModelAsset(const CWString& modelAssetPath)
	{
		resourceModelAssetPath_ = modelAssetPath;
	}

	void CMeshComponent::ReadyToDrawing()
	{
		if (resourceModelAssetPath_.length() > 0)
		{
			if (resourceModelAsset_ != nullptr)
				throw SMGEException(wtext("already ReadyToDrawing("));

			CResourceModel *resModel = nullptr;

			const auto assetKey = Globals::GetGameAssetPath(resourceModelAssetPath_);

			resourceModelAsset_ = CAssetManager::FindAsset<CResourceModel>(assetKey);
			if (resourceModelAsset_ == nullptr)
			{	// 없으면 로드
				//resourceModelAsset_ = CAssetManager::LoadAssetDefault<CResourceModel>(assetKey);
				resourceModelAsset_ = CAssetManager::LoadAssetCustom<CResourceModel>(assetKey,
					[asciiAssetKey = ToASCII(assetKey)]()
					{	// 커스텀 생성자
						auto resm = nsRE::CResourceModelProvider::FindResourceModel(asciiAssetKey);
						if (resm == nullptr)
							resm = nsRE::CResourceModelProvider::AddResourceModel(asciiAssetKey, std::make_shared<CResourceModel>(nullptr));
						return static_cast<CResourceModel*>(resm.get());
					},
					[]()
					{	// 커스텀 파괴자
						// 캐시에 그냥 남겨둠
					});

				resModel = resourceModelAsset_->getContentClass();
				resModel->OnAfterDeserialized2();	// 테스트 코드 - 임시 하드코드 - 지금은 뭔가 이상한 vftbl 관련 버그가 있어서 못쓰고 있다
			}
			else
				resModel = resourceModelAsset_->getContentClass();

			resModel->GetRenderModel(nullptr)->AddWorldObject(this);
		}

		Super::ReadyToDrawing();
	}

	SGReflection& CMeshComponent::getReflection()
	{
		if (reflMeshCompo_.get() == nullptr)
			reflMeshCompo_ = std::make_unique<TReflectionStruct>(*this);
		return *reflMeshCompo_.get();
	}
};
