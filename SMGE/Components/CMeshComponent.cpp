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
	DEFINE_RTTI_CObject_DEFAULT(CMeshComponent);
	DEFINE_RTTI_CObject_VARIETY(CMeshComponent, CObject*, const CWString&)

	CMeshComponent::CMeshComponent(CObject* outer) : CDrawComponent(outer)
	{
		className_ = wtext("SMGE::CMeshComponent");

		Ctor();
	}

	CMeshComponent::CMeshComponent(CObject* outer, const CWString& modelAssetPath) : CMeshComponent(outer)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CMeshComponent::Ctor()
	{
		isGameVisible_ = true;
#if IS_EDITOR
		isEditorVisible_ = true;
#endif
	}

	CMeshComponent::~CMeshComponent()
	{
		if (drawingModelAsset_ != nullptr)
		{	// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			auto smgeMA = drawingModelAsset_->getContentClass();
			GetRenderingEngine()->RemoveResourceModel(smgeMA);
		}
	}

	void CMeshComponent::SetDrawingModelAsset(const CWString& modelAssetPath)
	{
		drawingModelAssetPath_ = modelAssetPath;
	}

	void CMeshComponent::ReadyToDrawing()
	{
		if (drawingModelAssetPath_.length() > 0)
		{
			if (drawingModelAsset_ != nullptr)
				throw SMGEException(wtext("already ReadyToDrawing("));

			// 모델 애셋 로드
			drawingModelAsset_ = CAssetManager::LoadAsset<CResourceModel>(Globals::GetGameAssetPath(drawingModelAssetPath_));

			auto rsm = drawingModelAsset_->getContentClass();

			// 여기 수정 - 이거 CResourceModel 로 내리든가, 게임엔진에서 렌더링을 하도록 하자
			GetRenderingEngine()->AddResourceModel(drawingModelAssetPath_, rsm);

			rsm->GetRenderModel().AddWorldObject(this);
		}

		Super::ReadyToDrawing();
	}

	SGReflection& CMeshComponent::getReflection()
	{
		if (reflMeshCompo_.get() == nullptr)
			reflMeshCompo_ = MakeUniqPtr<TReflectionStruct>(*this);
		return *reflMeshCompo_.get();
	}
};
