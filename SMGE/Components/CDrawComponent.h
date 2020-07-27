#pragma once

#include "../GECommonIncludes.h"
#include "CComponent.h"
#include "../Assets/CAsset.h"
#include "../Assets/CAssetModel.h"
#include "../../MonoMax.EngineCore/RenderingEngine.h"

namespace SMGE
{
	class CDrawComponent : public CComponent, public nsRE::WorldModel
	{
	public:
		CDrawComponent(CObject* outer);
		CDrawComponent(CObject* outer, const CWString& modelAssetPath);

		virtual void Tick(float td);
		virtual void Render(float td);

		virtual void OnBeginPlay(class CActor* parent) override;
		virtual void OnEndPlay() override;

		virtual void ReadyToDrawing();
		void SetDrawingModelAsset(const CWString& modelAssetPath);

	//protected:
	//	class nsRE::CRenderingEngine* GetRenderingEngine();

	protected:
		// reflection
		CWString drawingModelAssetPath_;

	protected:
		// runtime
		CSharPtr<CAsset<CAssetModel>> drawingModelAsset_;
	};
};
