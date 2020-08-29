#pragma once

#include "../GECommonIncludes.h"
#include "CDrawComponent.h"
#include "../Assets/CResourceModel.h"

namespace SMGE
{
	class CMeshComponent;

	struct SGRefl_MeshComponent : public SGRefl_DrawComponent
	{
		using Super = SGRefl_DrawComponent;
		using TReflectionClass = CMeshComponent;

		SGRefl_MeshComponent(TReflectionClass& meshc);
		//SGRefl_MeshComponent(const CUniqPtr<CMeshComponent>& uptr);// { persistentComponentsREFL_ RTTI �ʿ� �̽�

		virtual operator CWString() const override;
		virtual SGReflection& operator=(CVector<TupleVarName_VarType_Value>& in) override;
		
		CWString& drawingModelAssetPath_;
	};

	class CMeshComponent : public CDrawComponent
	{
	public:
		using Super = CDrawComponent;
		using TReflectionStruct = SGRefl_MeshComponent;

		friend struct TReflectionStruct;

	public:
		CMeshComponent(CObject* outer);
		CMeshComponent(CObject* outer, const CWString& modelAssetPath);
		~CMeshComponent();

		void Ctor();

		virtual void ReadyToDrawing() override;

		void SetDrawingModelAsset(const CWString& modelAssetPath);

		// CInt_Reflection
		virtual SGReflection& getReflection() override;

	protected:
		CUniqPtr<TReflectionStruct> reflMeshCompo_;
		CWString drawingModelAssetPath_;

	protected:
		CSharPtr<CAsset<CResourceModel>> drawingModelAsset_;
	};
};
